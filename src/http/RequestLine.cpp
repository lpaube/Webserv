/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 13:06:08 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/06 20:42:10 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/RequestLine.hpp"
#include "Utils.hpp"
#include <algorithm>

namespace http
{
RequestLine::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

RequestLine::RequestLine()
    : method_(BAD_METHOD)
{
}

RequestLine::RequestLine(Buffer& request_data)
{
    std::string first_line = get_next_word(request_data, REQ_EOL, 2);
    std::string word = get_next_word(first_line, " ");

    method_ = method_from_str(word);
    if (method_ == BAD_METHOD) {
        std::string msg = "Bad http method: '" + word + "'";
        throw Exception(msg.c_str());
    }

    word = get_next_word(first_line, "?");
    if (!word.empty()) {
        path_ = word;
        query_str_ = get_next_word(first_line, " ");
    } else {
        path_ = get_next_word(first_line, " ");
    }

    http_version_ = first_line;
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
