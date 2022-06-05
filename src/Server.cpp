/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 21:22:23 by mleblanc         ###   ########.fr       */
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

// int main()
// {
//     SocketArray sockets;
//     std::vector<pollfd> pfds;

//     TcpStream* stream;
//     in_addr addr;
//     try {
//         addr.s_addr = inet_addr("127.0.0.1");
//         stream = new TcpStream(addr, 8000);
//         stream->init();
//         stream->bind();
//         stream->listen();
//         sockets.add(stream);
//         addr.s_addr = inet_addr("127.0.0.1");
//         stream = new TcpStream(addr, 9000);
//         stream->init();
//         stream->bind();
//         stream->listen();
//         sockets.add(stream);
//         addr.s_addr = inet_addr("127.0.0.1");
//         stream = new TcpStream(addr, 10000);
//         stream->init();
//         stream->bind();
//         stream->listen();
//         sockets.add(stream);
//     } catch (const std::exception& ex) {
//         std::cerr << ex.what() << std::endl;
//         return 1;
//     }

//     for (SocketArray::iterator it = sockets.begin(); it != sockets.end(); ++it) {
//         pollfd pfd;
//         pfd.fd = (*it)->fd();
//         pfd.events = POLLIN;
//         pfd.revents = 0;
//         pfds.push_back(pfd);
//     }

//     timeval timeout;
//     timeout.tv_sec = 15000;
//     timeout.tv_usec = 0;

//     for (;;) {
//         EventQueue events;

//         int ready = poll(pfds.data(), pfds.size(), 10000);

//         if (ready == -1) {
//             perror("Poll");
//             return 1;
//         }

//         std::vector<int> to_add;
//         std::vector<int> to_erase;
//         std::cout << "loop started" << std::endl;
//         for (auto& it : pfds) {
//             if (ready == 0) {
//                 break;
//             }

//             bool found = false;

//             if (it.revents & POLLIN) {
//                 found = true;

//                 SocketArray::iterator socket = sockets.find(it.fd);
//                 if (socket == sockets.end()) {
//                     std::cerr << "Did not find socket" << std::endl;
//                     continue;
//                 }

//                 switch ((*socket)->type()) {
//                     case TCP_STREAM: {
//                         Connection* c = new Connection(it.fd, timeout);
//                         // Maybe error
//                         c->init();
//                         sockets.add(c);
//                         to_add.push_back(c->fd());
//                         std::cout << "connection created" << std::endl;
//                         break;
//                     }
//                     case CONNECTION: {
//                         Connection& c = static_cast<Connection&>(**socket);
//                         char buf[4096];
//                         ssize_t bytes = recv(c.fd(), buf, 4096, 0);
//                         c.append_data(buf, buf + 4096);
//                         c.print_data();
//                         std::cout << "data read" << std::endl;
//                         break;
//                     }
//                 }
//             } else if (it.revents & POLLOUT) {
//                 found = true;

//                 const char* msg = "HTTP/1.0 200 OK\r\n\r\n<h1>Hello World Rust is the best "
//                                   "language ever made</h1>";
//                 send(it.fd, msg, strlen(msg), 0);
//                 to_erase.push_back(it.fd);
//                 std::cout << "sent response" << std::endl;
//             }

//             if (found) {
//                 --ready;
//             }
//         }

//         for (std::vector<int>::const_iterator it = to_erase.begin(); it != to_erase.end(); ++it) {
//             SocketArray::iterator socket = sockets.find(*it);
//             if (socket == sockets.end()) {
//                 std::cerr << "Can't find connection" << std::endl;
//                 continue;
//             }

//             sockets.erase(socket);

//             for (std::vector<pollfd>::iterator pfd = pfds.begin(); pfd != pfds.end(); ++pfd) {
//                 if (pfd->fd == *it) {
//                     pfds.erase(pfd);
//                     break;
//                 }
//             }
//         }

//         for (std::vector<int>::const_iterator it = to_add.begin(); it != to_add.end(); ++it) {
//             pollfd pfd = {};
//             pfd.fd = *it;
//             pfd.events = POLLIN | POLLOUT;
//             pfd.revents = 0;
//             pfds.push_back(pfd);
//         }
//     }
// }
