/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:19:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 11:51:54 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "HeaderMap.hpp"
#include "Method.hpp"
#include "QueryMap.hpp"
#include <unistd.h>

namespace http
{
class Request
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Request(std::string request_str);

public:
    ssize_t content_length() const;
    void set_body(const std::string& body);
    void print() const;

private:
    void parse_header(const Header& header);
    void parse_content_length(const std::string& value);
    void parse_transfer_encoding(std::string value);

private:
    Method method_;
    std::string path_;
    QueryMap query_;
    std::string http_version_;
    HeaderMap headers_;
    std::string body_;
    ssize_t content_length_;
    bool is_chunked_;
};

} // namespace http
