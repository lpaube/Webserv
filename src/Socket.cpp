/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 20:22:20 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:49:03 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <sys/socket.h>
#include <unistd.h>

Socket::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Socket::Socket(uint16_t port, in_addr address)
    : fd_(0), port_(port), addr_(address), is_init_(false)
{
}

Socket::~Socket()
{
    if (is_init_) {
        close(fd_);
    }
}

bool Socket::operator==(const Socket& rhs) const
{
    return address().s_addr == rhs.address().s_addr && port() == rhs.port();
}

bool Socket::operator==(int fd) const
{
    return this->fd() == fd;
}

void Socket::init()
{
    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1) {
        exception_errno<Exception>("Error while creating socket: ");
    }
    is_init_ = true;

    if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1) {
        exception_errno<Exception>("Error while setting socket to non-blocking: ");
    }

    int enable = 1;
    if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        exception_errno<Exception>("Error while setting address to reuse: ");
    }
}

void Socket::bind()
{
    check_init();

    sockaddr_in servaddr = {};
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr = address();
    servaddr.sin_port = htons(port());

    if (::bind(fd_, reinterpret_cast<sockaddr*>(&servaddr), sizeof(servaddr)) == -1) {
        exception_errno<Exception>("Error while binding socket: ");
    }
}

void Socket::listen()
{
    check_init();

    if (::listen(fd_, 32) == -1) {
        exception_errno<Exception>("Error while listening for connection: ");
    }
}

int Socket::fd() const
{
    return fd_;
}

uint16_t Socket::port() const
{
    return port_;
}

in_addr Socket::address() const
{
    return addr_;
}

void Socket::check_init() const
{
    if (!is_init_) {
        throw Exception("Socket is not initialized");
    }
}
