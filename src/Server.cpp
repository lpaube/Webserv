/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/22 14:42:40 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "TcpConnection.hpp"
#include "TcpListener.hpp"
#include <cstring>
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
            sockets_.insert(std::make_pair(listener->fd(), static_cast<Socket*>(listener)), POLLIN);

            streams.push_back(listener);

            std::cout << "Listening on " << listener->address_str() << ":";
            std::cout << listener->port() << std::endl;
        }
    }

    configs_ = blocks;
}

void Server::run()
{
	std::cerr << "IN RUN\n" << std::endl;
    signal(SIGINT, &stop_server);

    while (running) {
        int nevents = poll(sockets_.pfds(), sockets_.size(), -1);
        if (nevents < 0) {
            if (errno != EINTR) {
                std::cerr << "Poll error " << errno << std::endl;
            }
            continue;
        }

        std::vector<int> to_close;
        for (size_t i = 0; i < sockets_.size() && nevents > 0; ++i) {
            pollfd pfd = sockets_.pfds()[i];
            Socket* s = sockets_[pfd.fd];
            bool found = false;

            if (pfd.revents & (POLLERR | POLLHUP | POLLNVAL)) {
                pfd.revents |= (POLLIN | POLLOUT);
            }

            if ((pfd.revents & POLLIN) && (s->state() == S_READ)) {
                found = true;
                switch (s->type()) {
                    case TCP_LISTENER:
                        try {
                            accept_connection(static_cast<TcpListener*>(s));
                        } catch (const Request::Exception& ex) {

                        } catch (const std::exception& ex) {
                            std::cerr << ex.what() << std::endl;
                        }
                        break;
                    case TCP_CONNECTION: {
                        TcpConnection* c = static_cast<TcpConnection*>(s);
                        try {
                            c->handle_read_event();
                        } catch (const std::exception& ex) {
                            std::cerr << ex.what() << std::endl;
                            to_close.push_back(c->fd());
                        }
                        break;
                    }
                }
            }

            if ((pfd.revents & POLLOUT) && (s->state() == S_WRITE)) {
                found = true;
                TcpConnection* c = static_cast<TcpConnection*>(s);
                try {
                    c->handle_write_event(configs_);
                } catch (const std::exception& ex) {
                    std::cerr << ex.what() << std::endl;
                }
                // TODO: better write handling
                to_close.push_back(c->fd());
            }

            if (found) {
                --nevents;
            }
        }

        for (std::vector<int>::iterator it = to_close.begin(); it != to_close.end(); ++it) {
            close_connection(sockets_[*it]);
        }
        to_close.clear();
    }

    sockets_.close_all();
}

void Server::accept_connection(TcpListener* socket)
{
    TcpConnection* c = new TcpConnection(socket->fd());

    sockets_.insert(std::make_pair(c->fd(), static_cast<Socket*>(c)), POLLIN | POLLOUT);
	c->set_addr(socket->address());
	c->set_port(socket->port());
    std::cout << "Accepted connection: " << c->fd() << std::endl;
}

void Server::close_connection(Socket* c)
{
    close(c->fd());
    sockets_.erase(c->fd());
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
