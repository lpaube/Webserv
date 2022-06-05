/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:33:05 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Connection.hpp"
#include "Utils.hpp"
#include "event/ConnectionReadEvent.hpp"
#include "event/ConnectionWriteEvent.hpp"
#include "event/TcpStreamEvent.hpp"
#include "http/Request.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>

Server::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Server::Server() : configured_(false)
{
}

void Server::configure(const std::vector<Config>& blocks)
{
    sockets_.clear();

    if (blocks.empty()) {
        throw Exception("No server configuration");
    }

    std::vector<TcpStream> streams;
    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        in_addr address;
        address.s_addr = inet_addr(it->listen.address.c_str());
        if (address.s_addr == INADDR_NONE) {
            std::string msg = "Malformed address: '" + it->listen.address + "'";
            throw Exception(msg.c_str());
        }

        TcpStream s = TcpStream(address, it->listen.port);
        if (std::find(streams.begin(), streams.end(), s) == streams.end()) {
            streams.push_back(s);
        }
    }

    for (std::vector<TcpStream>::const_iterator it = streams.begin(); it != streams.end(); ++it) {
        sockets_.add(new TcpStream(*it));
    }

    init_tcp_streams();
    configured_ = true;
}

void Server::init_tcp_streams()
{
    for (SocketArray::iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        TcpStream& s = static_cast<TcpStream&>(**it);

        s.init();
        s.bind();
        s.listen();

        pollfd pfd;
        pfd.fd = s.fd();
        pfd.events = POLLIN;
        pfd.revents = 0;
        pfds_.push_back(pfd);
    }
}

void Server::run()
{
    if (!configured_) {
        throw Exception("Server is not configured");
    }

    while (true) {
        int n_ready = poll(pfds_.data(), pfds_.size(), POLL_TIMEOUT);
        if (n_ready == -1) {
            exception_errno<Exception>("Error while polling sockets: ");
        }

        for (std::vector<pollfd>::iterator it = pfds_.begin(); it != pfds_.end() && n_ready > 0; ++it) {
            if (n_ready == 0) {
                break;
            }

            bool found = false;

            SocketArray::iterator socket = sockets_.find(it->fd);
            if (socket == sockets_.end()) {
                // This should never happen
                std::cerr << "Did not find socket: " << it->fd << std::endl;
                continue;
            }

            if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
                // If there was an error, add POLLIN | POLLOUT to catch it in one of the handlers
                it->revents |= POLLIN | POLLOUT;
            }
            if (it->revents & (POLLIN | POLLRDHUP)) {
                found = true;

                switch ((*socket)->type()) {
                    case TCP_STREAM:
                        events_.push(new event::TcpStreamEvent(*socket));
                        break;
                    case CONNECTION:
                        events_.push(new event::ConnectionReadEvent(*socket));
                        break;
                }
            }
            if (it->revents & POLLOUT) {
                found = true;
                events_.push(new event::ConnectionWriteEvent(*socket));
            }

            if (found) {
                --n_ready;
            }
        }

        process_event_queue();
    }
}

void Server::process_event_queue()
{
    while (!events_.empty()) {
        event::Event* ev = events_.pop();

        switch (ev->type()) {
            case event::TCP_STREAM_EVENT: {
                const TcpStream& s = static_cast<const TcpStream&>(*ev->data());
                accept_connection(s);
                break;
            }
            case event::CONNECTION_READ_EVENT: {
                const Connection& c = static_cast<const Connection&>(*ev->data());
                (void)c;
                break;
            }
            case event::CONNECTION_WRITE_EVENT: {
                const Connection& c = static_cast<const Connection&>(*ev->data());
                (void)c;
                break;
            }
        }

        delete ev;
    }
}

void Server::accept_connection(const TcpStream& stream)
{
    Connection* c = new Connection(&stream);

    try {
        c->init();
        sockets_.add(static_cast<Socket*>(c));

        pollfd pfd;
        pfd.fd = c->fd();
        pfd.events = POLLIN | POLLRDHUP | POLLOUT;
        pfd.revents = 0;
        pfds_.push_back(pfd);
    } catch (const std::exception& ex) {
        delete c;
        std::cerr << ex.what() << std::endl;
    }
}

// void Server::poll_remove_closed_connections(const std::vector<int>& fds)
// {
//     for (std::vector<int>::const_iterator it = fds.begin(); it != fds.end(); ++it) {
//         SocketArray::iterator socket = sockets_.find(*it);
//         if (socket == sockets_.end()) {
//             std::cerr << "Can't find connection" << std::endl;
//             continue;
//         }

//         sockets_.erase(socket);

//         for (std::vector<pollfd>::iterator pfd = pfds_.begin(); pfd != pfds_.end(); ++pfd) {
//             if (pfd->fd == *it) {
//                 pfds_.erase(pfd);
//                 break;
//             }
//         }
//     }
// }
