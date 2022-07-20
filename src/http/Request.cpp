/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 19:04:31 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/20 14:20:10 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Utils.hpp"
#include <iostream>
#include <sstream>

Request::Exception::Exception(const std::string& msg, int code)
    : ExceptionBase(msg),
      status_code_(code)
{
}

int Request::Exception::status_code() const
{
    return status_code_;
}

Request::Request()
    : state_(REQ_METHOD),
      method_(BAD_METHOD),
      path_(),
      query_str_(),
      http_version_(),
      headers_(),
      buffer_(),
      cur_header_name_(),
      is_content_length(false),
      content_length_count_(0),
      is_chunked(false),
      cur_chunk_size_(-1),
      cnk_state_(CNK_SIZE),
      raw_body_(),
      body_()
{
}

ParseState Request::parse_state() const
{
    return state_;
}

void Request::set_parse_state(ParseState state)
{
    state_ = state;
}

Method Request::method() const
{
    return method_;
}

void Request::set_method(Method method)
{
    method_ = method;
}

const std::string& Request::path() const
{
    return path_;
}

void Request::set_path(const std::string& path)
{
    path_ = path;
}

const std::string& Request::query_str() const
{
    return query_str_;
}

void Request::set_query_str(const std::string& query)
{
    query_str_ = query;
}

const std::string& Request::http_version() const
{
    return http_version_;
}

void Request::set_http_version(const std::string& version)
{
    http_version_ = version;
}

const std::vector<char>& Request::body() const
{
    return body_;
}

bool Request::chunked() const
{
    return is_chunked;
}

ssize_t Request::cur_chunk_size() const
{
    return cur_chunk_size_;
}

bool Request::content_length() const
{
    return is_content_length;
}

size_t Request::content_length_count() const
{
    return content_length_count_;
}

const std::string& Request::cur_header() const
{
    return cur_header_name_;
}

void Request::set_cur_header(const std::string& name)
{
    cur_header_name_ = name;
}

void Request::add_header(const std::string& name, const std::string& value)
{
    headers_.insert(std::make_pair(to_lower(name), trim(value, " ")));
}

const std::string& Request::buffer() const
{
    return buffer_;
}

void Request::clear_buf()
{
    buffer_.clear();
}

void Request::append_buf(char c)
{
    buffer_.push_back(c);
}

void Request::set_state_and_clear_buf(ParseState state)
{
    state_ = state;
    buffer_.clear();
}

void Request::set_state_and_value(ParseState state, void (Request::*setter)(const std::string&))
{
    state_ = state;
    (this->*setter)(buffer());
    buffer_.clear();
}

const char* method_str(Method method)
{
    switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case DELETE:
            return "DELETE";
        case OPTIONS:
            return "OPTIONS";
        default:
            return "BAD METHOD";
    }
}

Method method_from_str(const std::string& str)
{
    if (method_str(GET) == str) {
        return GET;
    }
    if (method_str(POST) == str) {
        return POST;
    }
    if (method_str(DELETE) == str) {
        return DELETE;
    }
    if (method_str(OPTIONS) == str) {
        return OPTIONS;
    }
    return BAD_METHOD;
}

void Request::process_headers()
{
    typedef std::multimap<std::string, std::string>::const_iterator iter;

    iter it = headers_.find("content-length");
    is_content_length = it != headers_end();
    if (is_content_length) {
        std::pair<iter, iter> range = headers_.equal_range("content-length");
        if (range.second->first == "content-length") {
            throw Exception("Bad request: More than one Content-Length header", 400);
        }

        std::stringstream ss(it->second);
        ss >> content_length_count_;

        if (!ss.eof()) {
            throw Exception("Bad request: Bad Content-Length value", 400);
        }
    }

    it = headers_.find("transfer-encoding");
    if (it != headers_end()) {
        std::pair<iter, iter> range = headers_.equal_range("transfer-encoding");
        for (iter it2 = range.first; it2 != range.second; ++it2) {
            std::vector<std::string> values = split(it2->second, ',');
            for (std::vector<std::string>::iterator v = values.begin(); v != values.end(); ++v) {
                std::string trimmed = trim(*v, " ");
                if (trimmed == "chunked") {
                    is_chunked = true;
                } else {
                    throw Exception("Bad request: Unsupported encoding: " + trimmed, 400);
                }
            }
        }
    }

    if (is_content_length && is_chunked) {
        throw Exception("Bad request: Content-Length and chunked encoding are present", 400);
    }

    it = headers_.find("host");
    if (it != headers_end()) {
        std::pair<iter, iter> range = headers_.equal_range("host");
        ++range.first;
        if (range.first != range.second) {
            throw Exception("Bad request: Multiple Host headers are present", 400);
        }
    }
}

void Request::append_raw_body(const std::vector<char>& data)
{
    raw_body_.insert(raw_body_.end(), data.begin(), data.end());
}

void Request::decode_raw_body()
{
    if (!is_chunked) {
        body_ = raw_body_;
    } else {
        bool done = false;
        while (!done) {
            switch (cnk_state_) {
                case CNK_SIZE:
                    if (read_chunk_size()) {
                        if (cur_chunk_size_ == 0) {
                            done = true;
                            break;
                        }
                        cnk_state_ = CNK_CHUNK;
                    } else {
                        done = true;
                        break;
                    }
                    break;
                case CNK_CHUNK:
                    if (!read_chunk()) {
                        done = true;
                        break;
                    }
                    break;
                case CNK_NL:
                    if (raw_body_.size() >= 2) {
                        const_rbody_iter it = find_bytes(raw_body_, "\r\n", 2);
                        if (it != raw_body_.end() && it == raw_body_.begin()) {
                            cnk_state_ = CNK_SIZE;
                            raw_body_.erase(raw_body_.begin(), raw_body_.begin() + 2);
                        }
                    } else {
                        done = true;
                        break;
                    }
                    break;
            }
        }

        if (all_chunks_received()) {
            const_rbody_iter it = find_bytes(raw_body_, "\r\n", 2);
            if (it != raw_body_.end() && it == raw_body_.begin()) {
                raw_body_.clear();
            } else {
                throw Exception("Bad request: chunked request did not end with \\r\\n", 400);
            }
        }
    }
}

bool Request::read_chunk()
{
    ssize_t to_read =
        cur_chunk_size_ > (ssize_t)raw_body_.size() ? (ssize_t)raw_body_.size() : cur_chunk_size_;

    body_.insert(body_.end(), raw_body_.begin(), raw_body_.begin() + to_read);
    raw_body_.erase(raw_body_.begin(), raw_body_.begin() + to_read);
    cur_chunk_size_ -= to_read;

    bool read_all_chunk = false;
    if (cur_chunk_size_ == 0) {
        cur_chunk_size_ = -1;
        read_all_chunk = true;

        if (raw_body_.size() >= 2) {
            const_rbody_iter it = find_bytes(raw_body_, "\r\n", 2);
            if (it == raw_body_.end() || it != raw_body_.begin()) {
                throw Exception("Bad request: Chunk doesn't end with \\r\\n", 400);
            } else {
                raw_body_.erase(raw_body_.begin(), raw_body_.begin() + 2);
                cnk_state_ = CNK_SIZE;
            }
        } else {
            cnk_state_ = CNK_NL;
        }
    }

    return read_all_chunk;
}

bool Request::all_chunks_received() const
{
    return cur_chunk_size_ == 0;
}

bool Request::read_chunk_size()
{
    rbody_iter it = find_bytes(raw_body_, "\r\n", 2);
    if (it != raw_body_.end()) {
        std::string chunk_size(raw_body_.begin(), it);
        std::stringstream ss(chunk_size);
        ss << std::hex;
        ss >> cur_chunk_size_;
        if (!ss.eof()) {
            throw Exception("Bad chunk size: " + chunk_size, 400);
        }
        raw_body_.erase(raw_body_.begin(), it + 2);
        return true;
    }
    return false;
}

Request::header_iterator Request::find_header(const std::string& name) const
{
    return headers_.find(to_lower(name));
}

Request::header_iterator Request::headers_end() const
{
    return headers_.end();
}

void Request::print() const
{
    std::cout << "Method: " << method_str(method_) << "\n";
    std::cout << "Path: " << path_ << "\n";
    std::cout << "Query string: " << query_str_ << "\n";
    std::cout << "Http version: " << http_version_ << "\n";
    std::cout << "=====Headers====="
              << "\n";
    for (header_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << it->first << ": " << it->second << "\n";
    }
    std::cout << std::endl;
}
