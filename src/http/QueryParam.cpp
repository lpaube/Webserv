/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryParam.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:40:40 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 01:25:35 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/QueryParam.hpp"
#include "Utils.hpp"

namespace http
{

QueryParam::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

static void query_exception(const std::string& param)
{
    std::string msg = "Bad query param: '" + param + "'";
    throw QueryParam::Exception(msg.c_str());
}

QueryParam::QueryParam(const std::string& param)
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

const std::string& QueryParam::name() const
{
    return name_;
}

const std::string& QueryParam::value() const
{
    return value_;
}
} // namespace http
