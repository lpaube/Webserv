/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:19:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 00:33:06 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "HeaderMap.hpp"
#include "RequestLine.hpp"
#include <unistd.h>

namespace http
{
enum RequestState {
    REQ_LINE,
    REQ_HEADERS,
    REQ_BODY,
    REQ_DONE
};

class Request
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Request();
    Request(const RequestLine& request_line);

public:
    std::string& body();
    RequestLine& requestLine();
    void add_header(const Header& header);
    ssize_t content_length() const;
    void set_body(const std::string& body);
    void print() const;

private:
    void parse_header(const Header& header);
    void parse_content_length(const std::string& value);
    void parse_transfer_encoding(std::string value);

private:
    RequestLine request_line_;
    HeaderMap headers_;
    std::string body_;
    ssize_t content_length_;
    bool is_chunked_;
};

} // namespace http
