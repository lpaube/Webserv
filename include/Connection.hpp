/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Connection.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 16:47:12 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 00:57:12 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include <sys/socket.h>
#include <vector>

class Connection : public Socket
{
public:
    Connection(int host_fd, timeval timeout);

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
    int host_fd_;
    timeval timeout_;
    sockaddr addr_;
    socklen_t addrlen_;
    std::vector<char> buf_;
};
