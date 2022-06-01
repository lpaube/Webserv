/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 01:24:59 by mleblanc         ###   ########.fr       */
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
        headers_.add(Header(get_next_word(request_str, "\r\n")));
    }
}

ssize_t Request::content_length() const
{
    HeaderMap::const_iterator it = headers_.get(CONTENT_LENGTH_HEADER);
    if (it == headers_.end()) {
        return 0;
    }

    ssize_t size = 0;
    std::istringstream ss(it->second);
    ss >> size;

    if (ss.fail()) {
        return -1;
    }
    return size;
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

} // namespace http
