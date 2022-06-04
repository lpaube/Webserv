/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:51:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:02:34 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "ExceptionBase.hpp"
#include "ServerParser.hpp"
#include "Socket.hpp"
#include <poll.h>
#include <vector>

class Server
{
public:
    static const int POLL_TIMEOUT = 10000;
    static const int CONNECTION_TIMEOUT = 20;
    static const size_t BUFFER_SIZE = 1024 * 4;

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
    bool is_host(int fd) const;

private:
    std::vector<Socket> sockets_;
    std::vector<Client> clients_;
    std::vector<pollfd> pfds_;
};
