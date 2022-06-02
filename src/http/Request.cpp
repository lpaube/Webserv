/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 11:52:45 by mleblanc         ###   ########.fr       */
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

Request::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Request::Request(std::string request_str)
    : method_(BAD_METHOD), content_length_(0), is_chunked_(false)
{
    std::string request_line = get_next_word(request_str, "\r\n");
    std::string str = get_next_word(request_line, " ");

    method_ = method_from_str(str);
    if (method_ == BAD_METHOD) {
        std::string msg = "Bad http method: '" + str + "'";
        throw Exception(msg.c_str());
    }
    str = get_next_word(request_line, "?");
    if (!str.empty()) {
        path_ = str;
        query_ = QueryMap(get_next_word(request_line, " "));
    } else {
        path_ = get_next_word(request_line, " ");
    }
    http_version_ = request_line;

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
    std::cout << "Method: " << method_str(method_) << '\n';
    std::cout << "Path: " << path_ << '\n';
    std::cout << "Http version: " << http_version_ << '\n';
    std::cout << "Query params: \n";
    for (QueryMap::const_iterator it = query_.begin(); it != query_.end(); ++it) {
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
