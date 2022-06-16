/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:31:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 23:28:53 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "Request.hpp"
#include "Socket.hpp"
#include <sys/socket.h>
#include <vector>

class TcpConnection : public Socket
{
public:
    static const size_t MAX_REQUEST_SIZE = 1024 * 10;

public:
    TcpConnection(int listener_fd);

public:
    virtual SocketType type() const;

public:
    template <typename Iter>
    void append_data(Iter first, Iter last)
    {
        req_size_ += (size_t)std::distance(first, last);
        data_.insert(data_.end(), first, last);
    }

    void handle_read_event();
    void handle_write_event(const std::vector<Config>& server_configs);
    const Request& request() const;

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
    std::vector<Config> get_response_configs(const std::vector<Config>& server_configs) const;

private:
    int listener_fd_;
    sockaddr addr_;
    socklen_t addrlen_;
    std::vector<char> data_;
    void (TcpConnection::*request_handler)();
    size_t req_size_;
    Request req_;
};
