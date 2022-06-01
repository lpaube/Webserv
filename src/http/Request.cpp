/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:22:22 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "Utils.hpp"
#include "http/Method.hpp"
#include "http/Query.hpp"

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
        query_ = QueryList(get_next_word(request_line, " "));
    } else {
        path_ = get_next_word(request_line, " ");
    }
    http_version_ = request_line;

    while (request_str.find("\r\n") != 0) {
        headers_.add(Header(get_next_word(request_str, "\r\n")));
    }

    body_ = request_str;
}

} // namespace http
