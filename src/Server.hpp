/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 16:55:40 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 23:19:58 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "Sockets.hpp"
#include "TcpConnection.hpp"

class Server
{
public:
    static const int N_BACKLOG = 64;

public:
    Server(const std::vector<Config>& blocks);

public:
    void run();

private:
    void accept_connection(Socket* socket);
    void close_connection(Socket* c);
    void print_body(const Request& r) const;
    std::vector<Config> get_response_configs(const Request& req) const;

private:
    Sockets sockets_;
    std::vector<Config> configs_;
};
