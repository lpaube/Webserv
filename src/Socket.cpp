/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:12:06 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/13 00:41:38 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include <unistd.h>

Socket::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

Socket::Socket()
    : fd_(-1),
      state_(S_READ)
{
}

Socket::~Socket()
{
    if (is_init()) {
        close(fd());
    }
}

int Socket::fd() const
{
    return fd_;
}

bool Socket::is_init() const
{
    return fd_ != -1;
}

SocketState Socket::state() const
{
    return state_;
}

void Socket::set_state(SocketState state)
{
    state_ = state;
}
