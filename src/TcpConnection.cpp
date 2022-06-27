/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 21:52:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/27 15:24:43 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpConnection.hpp"
#include "Response.hpp"
#include "Script.hpp"
#include "Utils.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE (1024 * 8)

TcpConnection::TcpConnection(int listener_fd)
    : listener_fd_(listener_fd),
      request_handler(&TcpConnection::parse_http_request_line),
      req_size_(0),
      byte_sent(0)
{
    fd_ = accept(listener_fd_, (sockaddr*)&addr_, &addrlen_);
    if (fd() == -1) {
        throw Exception("Error while accepting connection: " + std::string(strerror(errno)));
    }

    if (fcntl(fd(), F_SETFL, O_NONBLOCK) == -1) {
        throw Exception("Error while setting socket to non-blocking: " +
                        std::string(strerror(errno)));
    }
}

SocketType TcpConnection::type() const
{
    return TCP_CONNECTION;
}

void TcpConnection::handle_read_event()
{
    char buf[BUF_SIZE];

    ssize_t n = recv(fd(), buf, BUF_SIZE, 0);

    if (n < 0) {
        throw Exception("Socket read error");
    }

    if (n == 0) {
        throw Exception("Client closed connection");
    }

    append_data(buf, buf + n);

    if (req_size_ > MAX_REQUEST_SIZE) {
        throw Exception("Request too big");
    }

    (this->*request_handler)();
}

bool TcpConnection::handle_write_event(const std::vector<Config>& server_configs)
{
    std::cout << "|!|IN_CONNECTION_WRITE_EVENT|!|" << std::endl;
    if (byte_sent != 0) {
        return send_response();
    }
    try {
        Config resp_configs = get_response_configs(server_configs);
    } catch (const std::exception& ex) {
        Response response(req_, server_configs[0]);
        response.set_status_code(500);
        response.check_error_code();
        response.set_html_header();
        msg = response.header + response.body;
        return (send_response());
    }
    Config resp_configs = get_response_configs(server_configs);
    std::size_t len = req_.path().length();
	std::cout << "PATH = " << req_.path() << "\n";
    if (req_.path().find("/cgi-bin/") != std::string::npos && req_.path()[len - 1] != '/') {
        std::cout << "|!|IN SCRIPT|!|" << std::endl;

        try {
            Script script(resp_configs, req_);
            msg = script.exec();
            return (send_response());
        } catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
            Response response(req_, resp_configs);
            std::string exec_msg("Error fatal, execve\n");
            std::string ext_msg("Error: No script extention found\n");
            if (exec_msg.compare(ex.what()) == 0 || ext_msg.compare(ex.what()) == 0) {
                response.set_status_code(404);
            } else {
                response.set_status_code(500);
            }
            response.check_error_code();
            response.set_html_header();
            msg = response.header + response.body;
            return (send_response());
        }
        std::cout << "|!|OUT OF SCRIPT|!|" << std::endl;
    } else {
        std::cout << "|!|IN FILE RESPONSE|!|" << std::endl;
        Response response(req_, resp_configs);
		std::cout << "FULL PATH =" << response.get_full_path() << std::endl;
        try {
            response.generate_response_html();
        } catch (const std::exception& ex) {
            std::cerr << ex.what() << std::endl;
            response.set_status_code(404);
            response.check_error_code();
            response.set_html_header();
            response.full_content = response.header + response.body;
        }
        msg = response.full_content;
        std::cout << "|!|FILE RESPONSE BUILT|!|" << std::endl;
        return (send_response()); // TODO: check err and if all bytes were sent
    }
}

const Request& TcpConnection::request() const
{
    return req_;
}

void TcpConnection::set_addr(in_addr addr)
{
    this->inaddr_ = addr;
}

void TcpConnection::set_port(uint16_t port)
{
    this->port_ = port;
}

void TcpConnection::parse_http_request_line()
{
    bool done = false;
    std::vector<char>::iterator it = data_.begin();
    for (; it != data_.end() && !done; ++it) {
        switch (req_.parse_state()) {
            case REQ_METHOD: {
                switch (*it) {
                    case ' ':
                        req_.set_parse_state(REQ_PATH);
                        req_.set_method(method_from_str(req_.buffer()));
                        req_.clear_buf();
                        if (req_.method() == BAD_METHOD) {
                            bad_request();
                        }
                        break;
                    case '\n':
                    case '\r':
                        bad_request();
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_PATH: {
                switch (*it) {
                    case '?':
                        req_.set_state_and_value(REQ_QUERY, &Request::set_path);
                        break;
                    case ' ':
                        req_.set_state_and_value(REQ_HTTP_VERSION, &Request::set_path);
                        break;
                    case '\n':
                    case '\r':
                        bad_request();
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_QUERY: {
                switch (*it) {
                    case ' ':
                        req_.set_state_and_value(REQ_HTTP_VERSION, &Request::set_query_str);
                        break;
                    case '\n':
                    case '\r':
                        bad_request();
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_HTTP_VERSION: {
                switch (*it) {
                    case '\r':
                        req_.set_state_and_value(REQ_LINE_ALMOST_DONE, &Request::set_http_version);
                        check_http_version();
                        break;
                    case '\n':
                        req_.set_state_and_value(REQ_HEADER_START, &Request::set_http_version);
                        check_http_version();
                        request_line_done(done);
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_LINE_ALMOST_DONE: {
                if (*it == '\n') {
                    req_.set_state_and_clear_buf(REQ_HEADER_START);
                    request_line_done(done);
                } else {
                    bad_request();
                }
            } break;
            default:
                throw Exception("Bad control flow");
        }
    }
    data_.erase(data_.begin(), it);
    if (req_.parse_state() == REQ_HEADER_START) {
        (this->*request_handler)();
    }
}

void TcpConnection::parse_http_request_headers()
{
    bool done = false;
    std::vector<char>::iterator it = data_.begin();
    for (; it != data_.end() && !done; ++it) {
        switch (req_.parse_state()) {
            case REQ_HEADER_START: {
                switch (*it) {
                    case '\r':
                        req_.set_state_and_clear_buf(REQ_ALL_HEADERS_ALMOST_DONE);
                        break;
                    case '\n':
                        headers_done(done);
                        break;
                    default:
                        req_.append_buf(*it);
                        req_.set_parse_state(REQ_HEADER_NAME);
                        break;
                }
            } break;
            case REQ_HEADER_NAME: {
                switch (*it) {
                    case ':':
                        req_.set_state_and_value(REQ_HEADER_VALUE, &Request::set_cur_header);
                        break;
                    case '\r':
                        add_header(REQ_HEADER_ALMOST_DONE);
                        break;
                    case '\n':
                        add_header(REQ_HEADER_START);
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_HEADER_VALUE: {
                switch (*it) {
                    case '\r':
                        add_header(REQ_HEADER_ALMOST_DONE);
                        break;
                    case '\n':
                        add_header(REQ_HEADER_START);
                        break;
                    default:
                        req_.append_buf(*it);
                        break;
                }
            } break;
            case REQ_HEADER_ALMOST_DONE: {
                if (*it == '\n') {
                    req_.set_state_and_clear_buf(REQ_HEADER_START);
                } else {
                    bad_request();
                }
            } break;
            case REQ_ALL_HEADERS_ALMOST_DONE: {
                if (*it == '\n') {
                    headers_done(done);
                } else {
                    bad_request();
                }
            } break;
            default:
                throw Exception("Bad control flow");
        }
    }
    data_.erase(data_.begin(), it);
    if (req_.parse_state() == REQ_BODY_START) {
        (this->*request_handler)();
    }
}

void TcpConnection::process_http_headers()
{
    if (req_.parse_state() != REQ_BODY_START) {
        throw Exception("Bad control flow");
    }

    req_.process_headers();
    req_.set_parse_state(REQ_BODY);
    request_handler = &TcpConnection::parse_http_request_body;
    (this->*request_handler)();
}

void TcpConnection::parse_http_request_body()
{
    if (req_.parse_state() != REQ_BODY) {
        throw Exception("Bad control flow");
    }

    if (req_.content_length()) {
        request_handler = &TcpConnection::parse_http_request_body_content_length;
    } else if (req_.chunked()) {
        request_handler = &TcpConnection::parse_http_request_body_chunked;
    } else {
        request_handler = &TcpConnection::parse_http_request_body_plain;
    }

    (this->*request_handler)();
}

void TcpConnection::parse_http_request_body_plain()
{
    req_.append_raw_body(data_);
    req_.decode_raw_body();
    data_.clear();
    set_state(S_WRITE);
}

void TcpConnection::parse_http_request_body_content_length()
{
    bool done = false;
    size_t n = data_.size();

    if (n == req_.content_length_count()) {
        done = true;
    } else if (n > req_.content_length_count()) {
        throw Request::Exception("Body larger than Content-Length");
    }

    req_.content_length_sub((size_t)n);

    if (done) {
        req_.append_raw_body(data_);
        data_.clear();
        req_.decode_raw_body();
        set_state(S_WRITE);
    }
}

void TcpConnection::parse_http_request_body_chunked()
{
    req_.append_raw_body(data_);
    data_.clear();
    req_.decode_raw_body();
    if (req_.all_chunks_received()) {
        print_bytes(req_.body());
        set_state(S_WRITE);
    }
}

void TcpConnection::bad_request() const
{
    throw Request::Exception("Bad request");
}

void TcpConnection::check_http_version() const
{
    if (req_.http_version() != "HTTP/1.1") {
        bad_request();
    }
}

void TcpConnection::request_line_done(bool& done)
{
    request_handler = &TcpConnection::parse_http_request_headers;
    done = true;
}

void TcpConnection::headers_done(bool& done)
{
    req_.set_state_and_clear_buf(REQ_BODY_START);
    request_handler = &TcpConnection::process_http_headers;
    done = true;
}

void TcpConnection::add_header(ParseState next_state)
{
    req_.add_header(req_.cur_header(), req_.buffer());
    req_.set_state_and_clear_buf(next_state);
}

// Compare the adress port of the connection with the list of configs.
// Match with the hostname, if no match found, the first adress/port match will be returned.
const Config& TcpConnection::get_response_configs(const std::vector<Config>& server_configs) const
{
    Request::header_iterator it = req_.find_header("host");
    int def = -1;
    if (it == req_.headers_end())
        throw Exception("Cannot find host");
    const std::string& host = it->second;
    in_addr addr_config;
    for (unsigned long i = 0; i < server_configs.size(); i++) {
        addr_config.s_addr = inet_addr(server_configs[i].listen.address.c_str());
        if (addr_config.s_addr == this->inaddr_.s_addr &&
            server_configs[i].listen.port == this->port_) {
            if (def == -1) {
                def = i;
            }
            for (std::size_t j = 0; j < server_configs[j].server_name.size(); j++) {
                if (host == server_configs[i].server_name[j]) {
                    return server_configs[i];
                }
            }
        }
    }
    if (def == -1) {
        throw Exception("No config match\n");
    }
    return server_configs[static_cast<std::size_t>(def)];
}

bool TcpConnection::send_response()
{

    size_t len = msg.length() - (size_t)byte_sent;
	std::cout << "PRINTING RESPONSE = \n" << msg.c_str() << std::endl;
    byte_sent = send(fd(), msg.c_str() + byte_sent, len, 0);
    if (byte_sent < 0) {
        throw Exception("Fatal, write return -1");
    }
    if (byte_sent == (ssize_t)len) {
        byte_sent = 0;
        return true;
    } else {
        return false;
    }
}
