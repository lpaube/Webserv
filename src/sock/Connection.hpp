/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 16:47:12 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/09 10:09:00 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Buffer.hpp"
#include "Socket.hpp"
#include "TcpListener.hpp"
#include "http/Request.hpp"
#include <sys/socket.h>

namespace sock
{
class Connection : public Socket
{
public:
    Connection(const TcpListener* stream, size_t buffer_size);

public:
    virtual void init();
    virtual SocketType type() const;

public:
    template <typename DataIter>
    void append_data(DataIter first, DataIter last)
    {
        buf_.append(first, last);
    }

    int host_fd() const;
    http::RequestState request_state() const;
    void next_request_state();
    Buffer& buffer();
    http::Request& request();

private:
    const TcpListener* stream_;
    sockaddr addr_;
    socklen_t addrlen_;
    Buffer buf_;
    http::RequestState request_state_;
    http::Request request_;
};
} // namespace sock
