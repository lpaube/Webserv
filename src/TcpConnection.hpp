/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:31:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/23 15:01:36 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

class TcpConnection : public Socket
{
public:
    static const size_t MAX_REQUEST_SIZE = 1024 * 10;

public:
    TcpConnection(int listener_fd);

    virtual SocketType type() const;

    template <typename Iter>
    void append_data(Iter first, Iter last)
    {
        req_size_ += (size_t)std::distance(first, last);
        data_.insert(data_.end(), first, last);
    }

    void handle_read_event();
    bool handle_write_event(const std::vector<Config>& server_configs);
    const Request& request() const;
    void set_addr(in_addr addr);
    void set_port(uint16_t port);

private:
    void parse_http_request_line();
    void parse_http_request_headers();
    void process_http_headers();
    void parse_http_request_body();
    void parse_http_request_body_plain();
    void parse_http_request_body_content_length();
    void parse_http_request_body_chunked();
    void bad_request() const;
    void check_http_version() const;
    void request_line_done(bool& done);
    void headers_done(bool& done);
    void add_header(ParseState next_state);
    bool send_response();
    const Config& get_response_configs(const std::vector<Config>& server_configs) const;

private:
    int listener_fd_;
    sockaddr addr_;
    uint16_t port_;
    socklen_t addrlen_;
    in_addr inaddr_;
    std::vector<char> data_;
    void (TcpConnection::*request_handler)();
    size_t req_size_;
    Request req_;
    std::string msg;
    ssize_t byte_sent;
};
