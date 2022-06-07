/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/06 19:03:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <unistd.h>

Connection::Connection(const TcpStream* stream, size_t buffer_size)
    : Socket(),
      stream_(stream),
      buf_(buffer_size),
      request_state_(http::REQ_LINE)
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
    return stream_->fd();
}

http::RequestState Connection::request_state() const
{
    return request_state_;
}

void Connection::set_request_state(http::RequestState state)
{
    request_state_ = state;
}

Buffer& Connection::buffer()
{
    return buf_;
}

http::Request& Connection::request()
{
    return request_;
}
