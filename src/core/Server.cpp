/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/18 15:46:19 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "fd/File.hpp"
#include "fd/TcpConnection.hpp"
#include "fd/TcpListener.hpp"
#include "http/Response.hpp"
#include <cstring>
#include <exception>
#include <iostream>
#include <signal.h>
#include <sys/errno.h>
#include <unistd.h>

bool running = true;

void stop_server(int sig)
{
    (void)sig;
    running = false;
    std::cout << "Stopping server..." << std::endl;
}

Server::Server(const std::vector<Config>& blocks)
    : fds_(),
      configs_(blocks)
{
    typedef std::vector<TcpListener*>::const_iterator s_iter;

    std::vector<TcpListener*> streams;
    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        bool add = true;
        for (s_iter s_it = streams.begin(); s_it != streams.end(); ++s_it) {
            if ((*s_it)->address_str() == it->listen.address &&
                (*s_it)->port() == it->listen.port) {
                add = false;
                break;
            }
        }

        if (add) {
            TcpListener* listener = new TcpListener(it->listen.address, it->listen.port, N_BACKLOG);
            fds_.insert(std::make_pair(listener->fd(), static_cast<FileDescriptor*>(listener)),
                        POLLIN);

            streams.push_back(listener);

            std::cout << "Listening on " << listener->address_str() << ":";
            std::cout << listener->port() << std::endl;
        }
    }
}

void Server::run()
{
    std::cerr << "IN RUN\n" << std::endl;
    signal(SIGINT, &stop_server);

    while (running) {
        int nevents = poll(fds_.pfds(), fds_.size(), -1);
        if (nevents < 0) {
            if (errno != EINTR) {
                std::cerr << "Poll error " << errno << std::endl;
            }
            continue;
        }

        std::vector<int> to_close;
        for (size_t i = 0; i < fds_.size() && nevents > 0; ++i) {
            pollfd pfd = fds_.pfds()[i];
            FileDescriptor* fd = fds_[pfd.fd];
            bool found = false;

            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                pfd.revents |= (POLLIN | POLLOUT);
            }

            if ((pfd.revents & POLLIN) && (fd->state() == S_READ)) {
                found = true;
                switch (fd->type()) {
                    case FD_TCP_LISTENER:
                        try {
                            accept_connection(static_cast<TcpListener*>(fd));
                        } catch (std::exception& ex) {
                            std::cerr << ex.what() << std::endl;
                        }
                        break;
                    case FD_TCP_CONNECTION: {
                        TcpConnection* c = static_cast<TcpConnection*>(fd);
                        try {
                            c->handle_read_event();
                        } catch (Request::Exception& ex) {
                            c->set_state(S_WRITE);
                            c->set_response_config(configs_, "");
                            error_response(c, ex.status_code());
                        } catch (std::exception& ex) {
                            std::cerr << ex.what() << std::endl;
                            to_close.push_back(c->fd());
                        }
                        break;
                    }
                    case FD_FILE: {
                        File* f = static_cast<File*>(fd);
                        f->handle();
                        break;
                    }
                }
            }

            if ((pfd.revents & POLLOUT) && (fd->state() == S_WRITE)) {
                found = true;
                switch (fd->type()) {
                    case FD_TCP_CONNECTION: {
                        bool sent = false;
                        TcpConnection* c = static_cast<TcpConnection*>(fd);

                        if (!c->has_config()) {
                            c->set_response_config(configs_, get_configuration(c));
                        }

                        try {
                            sent = c->handle_write_event(fds_);
                        } catch (Request::Exception& ex) {
                            error_response(c, ex.status_code());
                        } catch (std::exception& ex) {
                            std::cerr << ex.what() << std::endl;
                            to_close.push_back(c->fd());
                        }
                        if (sent) {
                            to_close.push_back(c->fd());
                        }
                    } break;
                    case FD_FILE: {
                        File* f = static_cast<File*>(fd);
                        f->handle();
                    } break;
                    case FD_TCP_LISTENER: {
                        // Should never happen
                    } break;
                }
            }

            if (found) {
                --nevents;
            }
        }

        for (std::vector<int>::iterator it = to_close.begin(); it != to_close.end(); ++it) {
            close_connection(fds_[*it]);
        }
        to_close.clear();
    }

    fds_.close_all();
}

void Server::accept_connection(TcpListener* socket)
{
    TcpConnection* c = new TcpConnection(socket->fd());

    fds_.insert(std::make_pair(c->fd(), static_cast<FileDescriptor*>(c)), POLLIN | POLLOUT);
    c->set_addr(socket->address());
    c->set_port(socket->port());
    std::cout << "Accepted connection: " << c->fd() << std::endl;
}

void Server::close_connection(FileDescriptor* c)
{
    close(c->fd());
    fds_.erase(c->fd());
    std::cout << "Closed connection: " << c->fd() << std::endl;
    delete c;
}

void Server::print_body(const Request& r) const
{
    for (std::vector<char>::const_iterator it = r.body().begin(); it != r.body().end(); ++it) {
        std::cout << *it;
    }
    std::cout << std::endl;
}

void Server::error_response(TcpConnection* c, int code)
{
    Response response(c->request(), c->config());
    response.set_status_code(code);
    response.check_error_code();
    response.set_html_header();
    std::vector<char> msg(response.header.begin(), response.header.end());
    msg.insert(msg.end(), response.body.begin(), response.body.end());
    c->set_msg(msg);
}

std::string Server::get_configuration(TcpConnection* c) const
{
    Request::header_iterator it = c->request().find_header("host");
    std::string host;
    if (it == c->request().headers_end()) {
        host = "";
    } else {
        host = it->second;
    }

    return host;
}
