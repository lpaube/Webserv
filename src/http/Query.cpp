/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:40:40 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:40:44 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Query.hpp"
#include "Utils.hpp"
#include <stdexcept>

namespace http
{
Query::Query(const std::string& str)
{
    std::string::size_type pos = str.find('=');
    if (pos == std::string::npos) {
        throw std::runtime_error("Bad query value");
    }
    name_ = str.substr(0, pos);
    value_ = str.substr(pos + 1);
    if (name_.empty()) {
        throw std::runtime_error("Bad query name");
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
