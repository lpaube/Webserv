/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryParam.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 17:24:45 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 01:24:38 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include <map>
#include <string>

namespace http
{
class QueryParam
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    QueryParam(const std::string& param);

public:
    const std::string& name() const;
    const std::string& value() const;

private:
    std::string name_;
    std::string value_;
};

} // namespace http
