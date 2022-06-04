/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 13:06:08 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 16:42:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/RequestLine.hpp"
#include "Utils.hpp"
#include <algorithm>

namespace http
{
RequestLine::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

RequestLine::RequestLine() : method_(BAD_METHOD)
{
}

RequestLine::RequestLine(std::string request_str)
{
    std::string word = get_next_word(request_str, " ");

    method_ = method_from_str(word);
    if (method_ == BAD_METHOD) {
        std::string msg = "Bad http method: '" + word + "'";
        throw Exception(msg.c_str());
    }

    word = get_next_word(request_str, "?");
    if (!word.empty()) {
        path_ = word;
        query_str_ = get_next_word(request_str, " ");
    } else {
        path_ = get_next_word(request_str, " ");
    }

    http_version_ = get_next_word(request_str, "\r\n");
    if (http_version_.empty()) {
        throw Exception("Bad request");
    }
    if (http_version_ != SUPPORTED_HTTP_VERSION) {
        std::string msg = "Unsupported http version: '" + word + "'";
        throw Exception(msg.c_str());
    }
}

Method RequestLine::method() const
{
    return method_;
}

const std::string& RequestLine::path() const
{
    return path_;
}

const std::string& RequestLine::query() const
{
    return query_str_;
}

const std::string& RequestLine::http_version() const
{
    return http_version_;
}

} // namespace http
