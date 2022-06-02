/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 13:20:46 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "Utils.hpp"
#include "http/Method.hpp"
#include "http/QueryParam.hpp"
#include <iostream>
#include <sstream>

namespace http
{

Request::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

Request::Request(const RequestLine& request_line, std::string request_str)
    : request_line_(request_line),
      content_length_(0),
      is_chunked_(false)
{
    std::string::size_type pos;
    while ((pos = request_str.find("\r\n")) != std::string::npos) {
        if (pos == 0) {
            break;
        }
        Header header(get_next_word(request_str, "\r\n"));
        headers_.add(header);
        parse_header(header);
    }
}

ssize_t Request::content_length() const
{
    return content_length_;
}

void Request::set_body(const std::string& body)
{
    body_ = body;
}

void Request::print() const
{
    std::cout << "Method: " << method_str(request_line_.method()) << '\n';
    std::cout << "Path: " << request_line_.path() << '\n';
    std::cout << "Http version: " << request_line_.http_version() << '\n';
    std::cout << "Query params: \n";
    for (QueryMap::const_iterator it = request_line_.query().begin();
         it != request_line_.query().end(); ++it) {
        std::cout << '\t' << it->first << " = " << it->second << '\n';
    }
    std::cout << "Headers: \n";
    for (HeaderMap::const_iterator it = headers_.begin(); it != headers_.end(); ++it) {
        std::cout << '\t' << it->first << ": " << it->second << '\n';
    }
    std::cout << "Body: " << body_ << std::endl;
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
    ssize_t size = 0;
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
