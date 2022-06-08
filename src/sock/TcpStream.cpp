/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStream.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 20:22:20 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 19:27:22 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpStream.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

namespace sock
{
TcpStream::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

TcpStream::TcpStream(in_addr address, uint16_t port)
    : Socket(),
      addr_(address),
      port_(port)
{
}

bool TcpStream::operator==(const TcpStream& rhs) const
{
    return address().s_addr == rhs.address().s_addr && port() == rhs.port();
}

// bool TcpStream::operator==(int fd) const
// {
//     return this->fd() == fd;
// }

void TcpStream::init()
{
    if (is_init()) {
        std::cout << "Socket already initialized" << std::endl;
        return;
    }

    fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1) {
        exception_errno<Exception>("Error while creating socket: ");
    }
    is_init_ = true;

    if (fcntl(fd_, F_SETFL, O_NONBLOCK) == -1) {
        exception_errno<Exception>("Error while setting socket to non-blocking: ");
    }

    int enable = 1;
    if (setsockopt(fd(), SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) == -1) {
        exception_errno<Exception>("Error while setting address to reuse: ");
    }
}

SocketType TcpStream::type() const
{
    return TCP_STREAM;
}

void TcpStream::bind()
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

void TcpStream::listen()
{
    check_init();

    if (::listen(fd_, 32) == -1) {
        exception_errno<Exception>("Error while listening for connection: ");
    }
}

uint16_t TcpStream::port() const
{
    return port_;
}

in_addr TcpStream::address() const
{
    return addr_;
}
} // namespace sock
