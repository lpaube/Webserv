/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:10:03 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 14:51:22 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Connection.hpp"
#include "Utils.hpp"
#include <fcntl.h>
#include <unistd.h>

namespace sock
{
Connection::Connection(const TcpListener* stream, size_t buffer_size)
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

void Connection::next_request_state()
{
    switch (request_state_) {
        case http::REQ_LINE:
            request_state_ = http::REQ_HEADERS;
            break;
        case http::REQ_HEADERS:
            request_state_ = http::REQ_BODY;
            break;
        case http::REQ_BODY:
            request_state_ = http::REQ_DONE;
            break;
        default:
            break;
    }
}

Buffer& Connection::buffer()
{
    return buf_;
}

http::Request& Connection::request()
{
    return request_;
}

sockaddr& Connection::addr(){
	return addr_;
}

} // namespace sock
