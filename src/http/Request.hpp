/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:19:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 18:55:09 by mleblanc         ###   ########.fr       */
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
    static const ssize_t MAX_REQ_BODY_SIZE = 1024 * 200;

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
    void add_header(const Header& header);
    size_t content_length() const;
    bool is_chunked() const;
    void read_body_bytes(size_t count);
    Buffer& body();
    void print() const;

private:
    void parse_header(const Header& header);
    void parse_content_length(const std::string& value);
    void parse_transfer_encoding(std::string value);

private:
    RequestLine request_line_;
    HeaderMap headers_;
    Buffer body_;
    size_t content_length_;
    bool is_chunked_;
};

} // namespace http
