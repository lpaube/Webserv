/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 19:04:31 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/16 15:22:54 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"
#include "Utils.hpp"
#include <sstream>

Request::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

Request::Request()
    : state_(REQ_METHOD),
      method_(BAD_METHOD),
      is_content_length(false),
      content_length_count_(0),
      is_chunked(false),
      cur_chunk_size_(-1)
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
	if (method_str(OPTIONS) == str){
		return OPTIONS;
	}
    return BAD_METHOD;
}

void Request::process_headers()
{
    typedef std::multimap<std::string, std::string>::const_iterator iter;

    iter it = headers_.find("content-length");
    is_content_length = it != headers_.end();
    if (is_content_length) {
        std::pair<iter, iter> range = headers_.equal_range("content-length");
        if (range.second->first == "content-length") {
            throw Exception("Bad request: More than one Content-Length header");
        }

        std::stringstream ss(it->second);
        ss >> content_length_count_;

        if (!ss.eof()) {
            throw Exception("Bad request: Bad Content-Length value");
        }
    }

    it = headers_.find("transfer-encoding");
    if (it != headers_.end()) {
        std::pair<iter, iter> range = headers_.equal_range("transfer-encoding");
        for (iter it2 = range.first; it2 != range.second; ++it2) {
            std::vector<std::string> values = split(it2->second, ',');
            for (std::vector<std::string>::iterator v = values.begin(); v != values.end(); ++v) {
                std::string trimmed = trim(*v, " ");
                if (trimmed == "chunked") {
                    is_chunked = true;
                } else {
                    throw Exception("Bad request: Unsupported encoding: " + trimmed);
                }
            }
        }
    }

    if (is_content_length && is_chunked) {
        throw Exception("Bad request: Content-Length and chunked encoding are present");
    }

    it = headers_.find("host");
    if (it != headers_.end()) {
        std::pair<iter, iter> range = headers_.equal_range("host");
        ++range.first;
        if (range.first != range.second) {
            throw Exception("Bad request: Multiple Host headers are present");
        }
    }
}

void Request::content_length_sub(size_t n)
{
    content_length_count_ -= n;
}

void Request::set_raw_body(const std::vector<char>& data)
{
    raw_body_ = data;
}

void Request::decode_raw_body()
{
    if (!is_content_length && !is_chunked) {
        body_ = raw_body_;
    } else if (is_content_length) {
        body_ = raw_body_;
    } else {
        body_ = raw_body_;
    }
}

Request::header_iterator Request::find_header(const std::string& name) const
{
    return headers_.find(to_lower(name));
}

Request::header_iterator Request::headers_end() const
{
    return headers_.end();
}
