/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 19:31:29 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "Utils.hpp"
#include "http/Method.hpp"
#include <iostream>
#include <sstream>

namespace http
{

Request::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

Request::Request()
    : body_(MAX_REQ_BODY_SIZE),
      body_type_(B_NONE),
      content_length_(0),
      is_chunked_(false)
{
}

Request::Request(const RequestLine& request_line)
    : request_line_(request_line),
      body_(MAX_REQ_BODY_SIZE),
      body_type_(B_NONE),
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

RequestBodyType Request::body_type() const
{
    return body_type_;
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
    } else if (header.name() == CONTENT_TYPE_HEADER) {
        parse_content_type(header.value());
    }
}

void Request::parse_content_length(const std::string& value)
{
    size_t size = 0;
    std::istringstream ss(value);
    ss >> size;

    if (ss.fail()) {
        throw Exception("Bad value for " CONTENT_LENGTH_HEADER ": " + value);
    }
    content_length_ = size;
    body_type_ = B_CONTENT_LENGTH;
}

void Request::parse_transfer_encoding(const std::string& value)
{
    std::string tmp = value;

    to_lower(tmp);
    if (tmp == "chunked") {
        is_chunked_ = true;
        body_type_ = B_CHUNKED;
    }
}

void Request::parse_content_type(const std::string& value)
{
    std::string tmp = value;
    std::string type = get_next_word(tmp, ";");

    if (type == "multipart/form-data") {
        std::string boundary = trim(tmp, " ");

        if (boundary.find("boundary=") == 0) {
            boundary_ = boundary.substr(8);
            body_type_ = B_MULTIPART_FORMDATA;
        } else {
            throw Exception("Bad multiform boundary: '" + boundary + "'");
        }
    }
}

} // namespace http
