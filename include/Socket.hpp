/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:14:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 19:18:03 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"

enum SocketType {
    TCP_STREAM,
    CONNECTION
};

class Socket
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Socket();
    virtual ~Socket();

public:
    virtual void init() = 0;
    virtual SocketType type() const = 0;

public:
    int fd() const;
    bool is_init() const;
    bool read() const;
    void set_write();

protected:
    void check_init() const;

protected:
    int fd_;
    bool is_init_;
    bool read_;
};
