/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:51:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/11 17:17:39 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "ServerParser.hpp"
#include "Config.hpp"
#include "event/EventQueue.hpp"
#include "sock/Connection.hpp"
#include "sock/SocketArray.hpp"
#include "sock/TcpStream.hpp"
#include <poll.h>
#include <vector>

class Server
{
public:
    static const int POLL_TIMEOUT = 5000;
    static const int CONNECTION_TIMEOUT = 20;
    static const size_t BUFFER_SIZE = 1024 * 10;
    static const ssize_t MAX_REQUEST_SIZE = 1024 * 200;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
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
    void accept_connection(const sock::TcpStream& stream);
    void receive_data(sock::Connection& connection);
    void close_connection(sock::Connection& c);

private:
    sock::SocketArray sockets_;
    std::vector<pollfd> pfds_;
    event::EventQueue events_;
	std::vector<Config> configList_;
    bool configured_;
};

std::vector<Config> getRespConfigs(sock::Connection c, std::vector<Config>& configList_);
