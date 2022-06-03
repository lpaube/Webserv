/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:51:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 18:31:37 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "ServerParser.hpp"
#include "SocketArray.hpp"

class Server
{
public:
    static const int POLL_TIMEOUT = 10000;
    static const int CONNECTION_TIMEOUT = 10;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    void configure(const std::vector<Config>& blocks);
    void run();

private:
    SocketArray sockets_;
};
