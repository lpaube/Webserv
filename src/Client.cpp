/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 23:09:22 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:54:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include <unistd.h>
#include "Utils.hpp"

Client::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Client::Client() : fd_(0), addr_(), addrlen_(0), is_init_(false)
{
}

Client::~Client()
{
    if (is_init_) {
        close(fd_);
    }
}
bool Client::operator==(int fd) const
{
    return this->fd() == fd;
}

void Client::accept(int server_fd, timeval timeout)
{
    fd_ = ::accept(server_fd, reinterpret_cast<sockaddr*>(&addr_), &addrlen_);
    if (fd_ == -1) {
        exception_errno<Exception>("Error while accepting connection: ");
    }
    is_init_ = true;

    if (setsockopt(fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
        exception_errno<Exception>("Error while setting timeout on connection: ");
    }
}

int Client::fd() const
{
    return fd_;
}


Client::data_iter Client::data_begin() const
{
    return data_.begin();
}

Client::data_iter Client::data_end() const
{
    return data_.end();
}
