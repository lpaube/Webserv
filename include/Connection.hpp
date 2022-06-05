/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 16:47:12 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:26:34 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include "TcpStream.hpp"
#include <sys/socket.h>
#include <vector>

class Connection : public Socket
{
public:
    Connection(const TcpStream* stream);

public:
    virtual void init();
    virtual SocketType type() const;

public:
    template <typename DataIter>
    void append_data(DataIter first, DataIter last)
    {
        buf_.insert(buf_.end(), first, last);
    }
    int host_fd() const;

private:
    const TcpStream* stream_;
    sockaddr addr_;
    socklen_t addrlen_;
    std::vector<char> buf_;
};
