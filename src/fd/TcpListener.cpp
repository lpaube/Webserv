/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListener.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:50:45 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 22:51:47 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpListener.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>

TcpListener::TcpListener(const std::string& address, uint16_t port, int backlog)
    : addr_str_(address),
      addr_(),
      port_(port),
      backlog_(backlog)
{
    addr_.s_addr = inet_addr(addr_str_.c_str());
    if (addr_.s_addr == INADDR_NONE) {
        throw Exception("Malformed address: '" + addr_str_ + "'");
    }

    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd() == -1) {
        throw Exception("Error while creating socket: " + std::string(strerror(errno)));
    }

    if (fcntl(fd(), F_SETFL, O_NONBLOCK) == -1) {
        throw Exception("Error while setting socket to non-blocking: " +
                        std::string(strerror(errno)));
    }

    int enable = 1;
    if (setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        throw Exception("Error while setting address to reuse: " + std::string(strerror(errno)));
    }

    sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr = addr_;
    servaddr.sin_port = htons(port_);

    if (bind(fd(), reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) == -1) {
        throw Exception("Error while binding socket: " + std::string(strerror(errno)));
    }

    if (listen(fd(), backlog_) == -1) {
        throw Exception("Error while listening for connection: " + std::string(strerror(errno)));
    }
}

FDType TcpListener::type() const
{
    return FD_TCP_LISTENER;
}

const std::string& TcpListener::address_str() const
{
    return addr_str_;
}

in_addr TcpListener::address() const
{
    return addr_;
}

uint16_t TcpListener::port() const
{
    return port_;
}
