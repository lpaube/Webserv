/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:51:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 14:52:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "ServerParser.hpp"
#include "Config.hpp"
#include "event/EventQueue.hpp"
#include "sock/Connection.hpp"
#include "sock/SocketArray.hpp"
#include "sock/TcpListener.hpp"
#include <poll.h>
#include <vector>

class Server
{
public:
    static const int POLL_TIMEOUT = 5000;
    static const int CONNECTION_TIMEOUT = 20;
    static const size_t BUFFER_SIZE = 1024 * 8;
    static const ssize_t MAX_REQ_SIZE = 1024 * 200000;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg);
    };

public:
    Server();

public:
    void configure(const std::vector<Config>& blocks);
    void run();

private:
    void init_tcp_streams();
    bool is_host(int fd) const;
    void process_event_queue();
    void accept_connection(const sock::TcpListener& stream);
    void receive_data(sock::Connection& connection);
    void close_connection(sock::Connection& c);

private:
    sock::SocketArray sockets_;
    std::vector<pollfd> pfds_;
    event::EventQueue events_;
    std::vector<Config> configList_;
    bool configured_;
};

std::vector<Config> getRespConfigs(http::HeaderMap header, std::vector<Config>& configList_);
