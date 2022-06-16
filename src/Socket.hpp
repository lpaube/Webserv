/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:08:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/13 00:41:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"

enum SocketType {
    TCP_LISTENER,
    TCP_CONNECTION
};

enum SocketState {
    S_READ,
    S_WRITE
};

class Socket
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg);
    };

public:
    Socket();
    virtual ~Socket();

public:
    virtual SocketType type() const = 0;

public:
    int fd() const;
    bool is_init() const;
    SocketState state() const;
    void set_state(SocketState state);

protected:
    int fd_;
    SocketState state_;

private:
    Socket(const Socket&);
    Socket& operator=(const Socket&);
};
