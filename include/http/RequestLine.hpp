/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestLine.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 12:59:13 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 14:28:10 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include "Method.hpp"
#include "QueryMap.hpp"
#include <cstddef>
#include <string>

#define SUPPORTED_HTTP_VERSION "HTTP/1.1"

namespace http
{
class RequestLine
{
public:
    static const std::size_t MAX_REQUEST_LINE_SIZE = 1024 * 8;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    RequestLine();
    RequestLine(std::string request_str);

public:
    Method method() const;
    const std::string& path() const;
    const QueryMap& query() const;
    const std::string& http_version() const;

private:
    Method method_;
    std::string path_;
    QueryMap query_;
    std::string http_version_;
};

} // namespace http
