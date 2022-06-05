/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 03:56:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <unistd.h>

Connection::Connection(int host_fd)
    : Socket(), host_fd_(host_fd)
{
}

void Connection::init()
{
    fd_ = accept(host_fd(), reinterpret_cast<sockaddr*>(&addr_), &addrlen_);
    if (fd_ == -1) {
        exception_errno<Exception>("Error while accepting connection: ");
    }
    is_init_ = true;

    if (fcntl(fd(), F_SETFL, O_NONBLOCK) == -1) {
        exception_errno<Exception>("Error while setting socket to non-blocking: ");
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
