/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:40:40 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:25:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Query.hpp"
#include "Utils.hpp"

namespace http
{

Query::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

static void query_exception(const std::string& param)
{
    std::string msg = "Bad query param: '" + param + "'";
    throw Query::Exception(msg.c_str());
}

Query::Query(const std::string& param)
{
    std::string::size_type pos = param.find('=');
    if (pos == std::string::npos) {
        query_exception(param);
    }
    name_ = param.substr(0, pos);
    value_ = param.substr(pos + 1);
    if (name_.empty()) {
        query_exception(param);
    }
}

const std::string& Query::name() const
{
    return name_;
}

const std::string& Query::value() const
{
    return value_;
}
} // namespace http
