/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 21:07:59 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Utils.hpp"
#include <unistd.h>

#include <iostream>

Connection::Connection(int host_fd, timeval timeout)
    : Socket(), host_fd_(host_fd), timeout_(timeout)
{
}

void Connection::init()
{
    fd_ = accept(host_fd(), reinterpret_cast<sockaddr*>(&addr_), &addrlen_);
    if (fd_ == -1) {
        exception_errno<Exception>("Error while accepting connection: ");
    }
    is_init_ = true;

    if (setsockopt(fd(), SOL_SOCKET, SO_RCVTIMEO, &timeout_, sizeof(timeout_)) == -1) {
        exception_errno<Exception>("Error while setting timeout on connection: ");
    }
}

SocketType Connection::type() const
{
    return CONNECTION;
}

int Connection::host_fd() const
{
    return host_fd_;
}

void Connection::print_data(){
    buf_.push_back(0);
    std::cout << buf_.data() << std::endl;
}