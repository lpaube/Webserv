/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 16:55:40 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/18 17:53:34 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core/Config.hpp"
#include "fd/FDList.hpp"
#include "fd/TcpConnection.hpp"
#include "fd/TcpListener.hpp"

class Server
{
public:
    static const int N_BACKLOG = 64;

public:
    Server(const std::vector<Config>& blocks);

    void run();

private:
    void accept_connection(SharedPtr<TcpListener> socket);
    void close_fd(SharedPtr<FileDescriptor> c);
    void print_body(const Request& r) const;
    std::vector<Config> get_response_configs(const Request& req) const;
    void error_response(SharedPtr<TcpConnection> c, int code);
    std::string get_configuration(SharedPtr<TcpConnection> c) const;

private:
    FDList fds_;
    std::vector<Config> configs_;
};
