/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketArray.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 17:44:19 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 18:38:27 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketArray.hpp"
#include <algorithm>
#include <cstdio>
#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include "Utils.hpp"

SocketArray::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

void SocketArray::add(uint16_t port, in_addr address)
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        exception_errno<Exception>("Error while creating socket: ");
    }

    sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr = address;
    servaddr.sin_port = htons(port);

    if (bind(socket_fd, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) == -1) {
        exception_errno<Exception>("Error while binding socket: ");
    }

    if (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1) {
        exception_errno<Exception>("Error while setting socket to non-blocking: ");
    }

    if (listen(socket_fd, 32) == -1) {
        exception_errno<Exception>("Error while listening for connection: ");
    }

    pollfd pfd = {};
    pfd.fd = socket_fd;
    pfd.events = POLLIN | POLLOUT;
    pollfds_.push_back(pfd);

    BindInfo binfo = {};
    binfo.fd = socket_fd;
    binfo.address = address;
    binfo.port = htons(port);
    binds_.push_back(binfo);
}

SocketArray::const_iterator SocketArray::find_match(int fd) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if (it->fd == fd) {
            return it;
        }
    }
    return end();
}

SocketArray::const_iterator SocketArray::find_match(uint16_t port, in_addr address) const
{
    for (const_iterator it = begin(); it != end(); ++it) {
        if (it->port == htons(port) && it->address.s_addr == address.s_addr) {
            return it;
        }
    }
    return end();
}

SocketArray::const_iterator SocketArray::begin() const
{
    return binds_.begin();
}

SocketArray::const_iterator SocketArray::end() const
{
    return binds_.end();
}

pollfd* SocketArray::pfd_array()
{
    return pollfds_.data();
}

std::vector<pollfd>::size_type SocketArray::pfd_size()
{
    return pollfds_.size();
}

SocketArray::poll_iterator SocketArray::pfd_begin()
{
    return pollfds_.begin();
}

SocketArray::poll_iterator SocketArray::pfd_end()
{
    return pollfds_.end();
}

SocketArray::~SocketArray()
{
    for (const_iterator it = begin(); it != end(); ++it) {
        close(it->fd);
    }
}
