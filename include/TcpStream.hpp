/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStream.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 20:13:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 18:29:51 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "Socket.hpp"
#include <netinet/in.h>
#include <vector>

class TcpStream : public Socket
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    TcpStream(in_addr address, uint16_t port);

public:
    bool operator==(const TcpStream& rhs) const;
    bool operator==(int fd) const;

public:
    virtual void init();

public:
    void bind();
    void listen();
    // Port in host byte order
    uint16_t port() const;
    in_addr address() const;

private:
    in_addr addr_;
    uint16_t port_;
};
