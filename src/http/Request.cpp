/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 18:55:39 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "Utils.hpp"
#include "http/Method.hpp"
#include <iostream>
#include <sstream>

namespace http
{

Request::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

Request::Request()
    : body_(MAX_REQ_BODY_SIZE),
      content_length_(0),
      is_chunked_(false)
{
}

Request::Request(const RequestLine& request_line)
    : request_line_(request_line),
      body_(MAX_REQ_BODY_SIZE),
      content_length_(0),
      is_chunked_(false)
{
}

void Request::add_header(const Header& header)
{
    headers_.add(header);
    parse_header(header);
}

size_t Request::content_length() const
{
    return content_length_;
}

bool Request::is_chunked() const
{
    return is_chunked_;
}

void Request::read_body_bytes(size_t count)
{
    size_t n = count > content_length_ ? content_length_ : count;
    content_length_ -= n;
}

Buffer& Request::body()
{
    return body_;
}

void Request::print() const
{
    std::cout << "Method: " << method_str(request_line_.method()) << '\n';
    std::cout << "Path: " << request_line_.path() << '\n';
    std::cout << "Http version: " << request_line_.http_version() << '\n';
    std::cout << "Query string: " << request_line_.query() << '\n';
    std::cout << "Headers: \n";
    for (HeaderMap::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << '\t' << it->first << ": " << it->second << '\n';
    }
}

void Request::parse_header(const Header& header)
{
    if (header.name() == CONTENT_LENGTH_HEADER) {
        parse_content_length(header.value());
    } else if (header.name() == TRANSFER_ENCODING_HEADER) {
        parse_transfer_encoding(header.value());
    }
}

void Request::parse_content_length(const std::string& value)
{
    size_t size = 0;
    std::istringstream ss(value);
    ss >> size;

    if (ss.fail()) {
        std::string msg = "Bad value for " CONTENT_LENGTH_HEADER ": " + value;
        throw Exception(msg.c_str());
    }
    content_length_ = size;
}

void Request::parse_transfer_encoding(std::string value)
{
    to_lower(value);
    if (value == "chunked") {
        is_chunked_ = true;
    }
}

} // namespace http
