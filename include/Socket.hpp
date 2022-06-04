/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 20:13:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:49:10 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include <netinet/in.h>
#include <vector>

class Socket
{
public:
    typedef std::vector<char>::size_type size_type;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Socket(uint16_t port, in_addr address);
    ~Socket();

public:
    bool operator==(const Socket& rhs) const;
    bool operator==(int fd) const;

public:
    void init();
    void bind();
    void listen();
    int fd() const;
    // Port in host byte order
    uint16_t port() const;
    in_addr address() const;

private:
    void check_init() const;

private:
    int fd_;
    uint16_t port_;
    in_addr addr_;
    bool is_init_;
};
