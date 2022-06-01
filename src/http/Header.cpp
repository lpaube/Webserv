/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:34:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:17:50 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Header.hpp"
#include "Utils.hpp"

namespace http
{

Header::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Header::Header(const std::string& str)
{
    std::string::size_type pos = str.find(':');
    if (pos == std::string::npos) {
        std::string msg = "Bad Header: '" + str + "'";
        throw Exception(msg.c_str());
    }
    name_ = str.substr(0, pos);
    value_ = trim(str.substr(pos + 1), ' ');
}

Header::Header(const std::string& n, const std::string& v) : name_(n), value_(v)
{
}

const std::string& Header::name() const
{
    return name_;
}

const std::string& Header::value() const
{
    return value_;
}
} // namespace http
