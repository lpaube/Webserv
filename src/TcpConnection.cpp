/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 21:52:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/18 16:38:22 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpConnection.hpp"
#include "Response.hpp"
#include "Script.hpp"
#include "Utils.hpp"
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUF_SIZE (1024 * 8)

TcpConnection::TcpConnection(int listener_fd)
    : listener_fd_(listener_fd),
      request_handler(&TcpConnection::parse_http_request_line),
      req_size_(0)
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

    ssize_t n = read(fd(), buf, BUF_SIZE);

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

void TcpConnection::handle_write_event(const std::vector<Config>& server_configs)
{
    std::cout << "|!|IN_CONNECTION_WRITE_EVENT|!|" << std::endl;

    std::vector<Config> resp_configs = server_configs;

    std::string msg;
	std::size_t len = req_.path().length();
    if (req_.path().find("cgi-bin/") != std::string::npos && req_.path()[len - 1] != '/') {
        std::cout << "|!|IN SCRIPT|!|" << std::endl;

        try{
			Script script(resp_configs[0], req_);
			msg = script.exec();
			if (write(fd(), msg.c_str(), msg.length()) < 0){
				throw Exception("Error fatal, write");
			}
		} catch(const std::exception& ex){
			std::cout << ex.what() << std::endl;
			Response response(req_, resp_configs);
			std::string exec_msg("Error fatal, execve\n");
			std::string ext_msg("Error: No script extention found\n");
			if (exec_msg.compare(ex.what()) == 0 || ext_msg.compare(ex.what()) == 0){
				response.setStatusCode(404);
			}
			else{
				response.setStatusCode(500);
			}
          	response.checkErrorCode();
          	response.setHtmlHeader();
          	msg = response.header + response.body;
			write(fd(), msg.c_str(), msg.length());
	}
	Script script(resp_configs[0], req_);
	msg = script.exec();
        std::cout << "|!|OUT OF SCRIPT|!|" << std::endl;
    } else {
        std::cout << "|!|IN FILE RESPONSE|!|" << std::endl;

        struct stat info;
        if (stat(req_.path().c_str(), &info) < 0) {
            // TODO: Error case
        }

        Response response(req_, resp_configs);
        /*
        if (response.has_return_redirect())
        {
          response.redirect();
        }
        */
        if (response.getMethod() == GET)
        {
          if (response.method_allowed(GET) == false)
          {
            std::cerr << "===GET method not allowed===" << std::endl;
            response.setStatusCode(405);
          }
          else
            response.setHtmlBody();
          std::cerr << "THIS IS BODY: " << response.body << std::endl;
          response.checkErrorCode();
          response.setHtmlHeader();
          response.full_content = response.header + response.body;
        }
        else if (response.getMethod() == DELETE)
        {
          if (response.method_allowed(DELETE) == false)
          {
            std::cerr << "===DELETE method not allowed===" << std::endl;
            response.setStatusCode(405);
          }
          else
          {
            response.remove_file();
            //response.setHtmlBody();
          }
          response.checkErrorCode();
          response.setHtmlHeader();
          response.full_content = response.header + response.body;
        }

        msg = response.full_content;
		write(fd(), msg.c_str(), msg.length()); // TODO: check err and if all bytes were sent
        std::cout << "|!|FILE RESPONSE BUILT|!|" << std::endl;
    }
}

const Request& TcpConnection::request() const
{
    return req_;
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
    req_.set_raw_body(data_);
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
        req_.set_raw_body(data_);
        data_.clear();
        req_.decode_raw_body();
        set_state(S_WRITE);
    }
}

void TcpConnection::parse_http_request_body_chunked()
{
    req_.set_raw_body(data_);
    data_.clear();
    req_.decode_raw_body();
    set_state(S_WRITE);
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

std::vector<Config>
TcpConnection::get_response_configs(const std::vector<Config>& server_configs) const
{
    std::vector<Config> response_configs;
    Request::header_iterator it = req_.find_header("host");

    // TODO: if it == end();

    const std::string& host = it->second;
    std::cout << "============This is host: " << host << "=============" << std::endl;
    for (unsigned long i = 0; i < server_configs.size(); i++) {
        if (host == server_configs[i].listen.combined) {
            response_configs.push_back(server_configs[i]);
        }
    }
    return response_configs;
}
