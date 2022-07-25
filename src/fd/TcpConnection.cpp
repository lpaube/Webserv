/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpConnection.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 21:52:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/25 12:56:16 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TcpConnection.hpp"
#include "File.hpp"
#include "http/Response.hpp"
#include "http/Script.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUF_SIZE (1024 * 8)
#define IN_TMPFILE "intmpfile.tmp"
#define OUT_TMPFILE "outtmpfile.tmp"

TcpConnection::TcpConnection(int listener_fd)
    : script_(NULL),
      listener_fd_(listener_fd),
      addr_(),
      addrlen_(),
      inaddr_(),
      data_(),
      request_handler(&TcpConnection::parse_http_request_line),
      req_size_(0),
      req_(),
      msg_(),
      byte_sent_(0),
      config_(),
      has_config_(false),
      file_(),
      size_checked_(false),
      get_config_(false),
      body_bytes_(0),
      script_started_(false),
      script_id_(0)
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

FDType TcpConnection::type() const
{
    return FD_TCP_CONNECTION;
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

    (this->*request_handler)();
}

bool TcpConnection::handle_write_event(FDList& fds)
{
    if (!size_checked_ && req_.body().size() > config_.client_max_body_size) {
        size_checked_ = true;
        throw Request::Exception("Body too big", 413);
    }
    if (byte_sent_ != 0 || msg_.size() > 0) {
        return send_response();
    }
    Response response(req_, config_);
    response.check_method();
    std::size_t len = req_.path().length();
    if (req_.path().find("/cgi-bin/") != std::string::npos && req_.path()[len - 1] != '/') {
        if (file_.get() == NULL) {
            file_ = SharedPtr<File>(new File(IN_TMPFILE, S_WRITE));
            file_->append_write_data(req_.body());
            fds.insert(std::make_pair(file_->fd(), static_pointer_cast<FileDescriptor>(file_)),
                       POLLOUT);
        } else if (file_->write_done()) {
            if (script_started_ == false) {
                script_ = new Script(config_, req_);
                if (script_->ext_found == true) {
                    script_id_ = script_->exec(IN_TMPFILE);
                    script_started_ = true;
                }
            }
            if (script_started_ == true) {
                int status;
                waitpid(script_id_, &status, WNOHANG);
                if ((WIFEXITED(status))) {
                    int mode = 0;
                    if (req_.method() == POST) {
                        mode = 1;
                    }
                    script_->close_files(mode);
                    file_ = SharedPtr<File>(new File(OUT_TMPFILE, S_READ));
                    fds.insert(
                        std::make_pair(file_->fd(), static_pointer_cast<FileDescriptor>(file_)),
                        POLLIN);
                    delete script_;
                    script_ = NULL;
                }
            }
        } else if (file_->read_done()) {
            msg_ = file_->get_read_data();
            file_.reset();
            return (send_response());
        } else if (file_->error()) {
            Response response(request(), config());
            response.set_status_code(500);
            response.check_error_code();
            response.set_html_header();
            std::vector<char> msg(response.header.begin(), response.header.end());
            msg.insert(msg.end(), response.body.begin(), response.body.end());
            return send_response();
        }
        return false;
    }
    response.generate_response_html();
    msg_ = response.full_content;
    return (send_response());
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

void TcpConnection::set_msg(const std::vector<char>& set)
{
    this->msg_ = set;
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

    get_config_ = true;

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
    req_.print();
}

void TcpConnection::parse_http_request_body_content_length()
{
    bool done = false;
    body_bytes_ += data_.size();

    if (has_config()) {
        if (req_.content_length_count() > config_.client_max_body_size) {
            throw Request::Exception("Body too big", 413);
        }
    }

    if (body_bytes_ == req_.content_length_count()) {
        done = true;
    } else if (body_bytes_ > req_.content_length_count()) {
        throw Request::Exception("Body larger than Content-Length", 400);
    }

    req_.append_raw_body(data_);
    data_.clear();

    if (done) {
        req_.decode_raw_body();
        set_state(S_WRITE);
        req_.print();
    }
}

void TcpConnection::parse_http_request_body_chunked()
{
    req_.append_raw_body(data_);
    data_.clear();
    req_.decode_raw_body();

    if (has_config()) {
        if (req_.body().size() > config_.client_max_body_size) {
            throw Request::Exception("Body too big", 413);
        }
    }

    if (req_.all_chunks_received()) {
        set_state(S_WRITE);
        req_.print();
    }
}

void TcpConnection::bad_request() const
{
    throw Request::Exception("Bad request", 400);
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
void TcpConnection::set_response_config(const std::vector<Config>& server_configs, std::string host)
{
    in_addr addr_config;
    int def = -1;

    for (std::size_t i = 0; i < server_configs.size(); i++) {
        addr_config.s_addr = inet_addr(server_configs[i].listen.address.c_str());
        if (addr_config.s_addr == this->inaddr_.s_addr &&
            server_configs[i].listen.port == this->port_) {
            if (def == -1) {
                def = i;
            }
            for (std::size_t j = 0; j < server_configs[i].server_name.size(); j++) {
                if (host == server_configs[i].server_name[j]) {
                    config_ = server_configs[i];
                    has_config_ = true;
                    get_config_ = false;
                    return;
                }
            }
        }
    }
    if (def == -1) {
        throw Request::Exception("No config match\n", 500);
    }
    config_ = server_configs[static_cast<std::size_t>(def)];
    has_config_ = true;
    get_config_ = false;
    return;
}

bool TcpConnection::send_response()
{
    size_t len = msg_.size() - (size_t)byte_sent_;
    byte_sent_ = send(fd(), msg_.data() + byte_sent_, len, 0);
    if (byte_sent_ < 0) {
        throw Exception("Error fatal, write returns -1");
    }
    if (byte_sent_ == (ssize_t)len) {
        byte_sent_ = 0;
        msg_.clear();
        return true;
    } else {
        return false;
    }
}

const Config& TcpConnection::config() const
{
    return this->config_;
}

bool TcpConnection::has_config() const
{
    return has_config_;
}

bool TcpConnection::get_config() const
{
    return get_config_;
}

TcpConnection::~TcpConnection()
{
    delete script_;
}
