/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:31:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/25 11:51:54 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Utils.hpp"
#include "core/Config.hpp"
#include "fd/FDList.hpp"
#include "fd/File.hpp"
#include "fd/FileDescriptor.hpp"
#include "http/Request.hpp"
#include "http/Script.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <vector>

class TcpConnection : public FileDescriptor
{
public:
    static const size_t MAX_REQUEST_SIZE = 0x8000000;

public:
    TcpConnection(int listener_fd);
    ~TcpConnection();
    virtual FDType type() const;

    template <typename Iter>
    void append_data(Iter first, Iter last)
    {
        req_size_ += (size_t)std::distance(first, last);
        data_.insert(data_.end(), first, last);
    }

    void handle_read_event();
    bool handle_write_event(FDList& fds);
    const Request& request() const;
    void set_addr(in_addr addr);
    void set_port(uint16_t port);
    void set_msg(const std::vector<char>& set);
    void set_response_config(const std::vector<Config>& server_configs, std::string host);
    const Config& config() const;
    bool has_config() const;
    bool get_config() const;

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

private:
    Script* script_;
    int listener_fd_;
    sockaddr addr_;
    uint16_t port_;
    socklen_t addrlen_;
    in_addr inaddr_;
    std::vector<char> data_;
    void (TcpConnection::*request_handler)();
    size_t req_size_;
    Request req_;
    std::vector<char> msg_;
    ssize_t byte_sent_;
    Config config_;
    bool has_config_;
    SharedPtr<File> file_;
    bool size_checked_;
    bool get_config_;
    size_t body_bytes_;
    bool script_started_;
    pid_t script_id_;
};
