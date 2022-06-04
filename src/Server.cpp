/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:57:08 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "Utils.hpp"
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

void Server::configure(const std::vector<Config>& blocks)
{
    sockets_.clear();

    if (blocks.empty()) {
        throw Exception("No server configuration");
    }

    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        in_addr address;
        address.s_addr = inet_addr(it->listen.address.c_str());
        if (address.s_addr == INADDR_NONE) {
            std::string msg = "Malformed address: '" + it->listen.address + "'";
            throw Exception(msg.c_str());
        }

        if (std::find(sockets_.begin(), sockets_.end(), Socket(it->listen.port, address)) ==
            sockets_.end()) {

            sockets_.push_back(Socket(it->listen.port, address));
            Socket& socket = sockets_.back();
            socket.init();
            socket.bind();
            socket.listen();

            pollfd pfd = {};
            pfd.fd = socket.fd();
            pfd.events = POLLIN;
            pfds_.push_back(pfd);
        }
    }
}

void Server::run()
{
    while (true) {
        int presult = poll(pfds_.data(), pfds_.size(), POLL_TIMEOUT);
        if (presult == -1) {
            exception_errno<Exception>("Error while polling sockets: ");
        }

        std::vector<int> to_add;
        std::vector<int> to_remove;
        for (std::vector<pollfd>::const_iterator it = pfds_.begin(); it != pfds_.end(); ++it) {
            if (it->revents & POLLIN) {
                if (is_host(it->fd)) {
                    timeval timeout = {};
                    timeout.tv_sec = CONNECTION_TIMEOUT;

                    clients_.push_back(Client());
                    Client& client = clients_.back();

                    try {
                        client.accept(it->fd, timeout);
                    } catch (const std::exception& ex) {
                        clients_.pop_back();
                        std::cerr << ex.what() << std::endl;
                        continue;
                    }

                    to_add.push_back(client.fd());
                } else {
                    char buf[BUFFER_SIZE];
                    ssize_t nbytes = recv(it->fd, buf, BUFFER_SIZE, 0);
                    if (nbytes <= 0) {
                        if (nbytes == 0) {
                            std::cerr << "Connection closed: socket " << it->fd << std::endl;
                        } else {
                            perror("recv");
                        }

                        close(it->fd);
                        to_remove.push_back(it->fd);
                    }

                    std::vector<Client>::iterator client_it =
                        std::find(clients_.begin(), clients_.end(), it->fd);

                    if (client_it == clients_.end()) {
                        // Should never happen
                        std::cerr << "Couldn't find client" << std::endl;
                        continue;
                    }

                    client_it->append_data(buf, buf + BUFFER_SIZE);
                }
            } else if (it->revents & POLLOUT) {
            }
        }

        for (std::vector<int>::const_iterator it = to_remove.begin(); it != to_remove.end(); ++it) {
            for (std::vector<pollfd>::iterator pit = pfds_.begin(); pit != pfds_.end(); ++pit) {
                if (pit->fd == *it) {
                    pfds_.erase(pit);
                    break;
                }
            }
        }
        for (std::vector<int>::const_iterator it = to_add.begin(); it != to_add.end(); ++it) {
            pollfd pfd = {};
            pfd.fd = *it;
            pfd.events = POLLIN | POLLOUT;
            pfds_.push_back(pfd);
        }
    }
}

bool Server::is_host(int fd) const
{
    return std::find(sockets_.begin(), sockets_.end(), fd) != sockets_.end();
}
