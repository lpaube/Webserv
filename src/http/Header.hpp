/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 16:38:07 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 19:24:17 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include <string>

namespace http
{
class Header
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg);
    };

public:
    Header(const std::string& str);
    Header(const std::string& n, const std::string& v);

public:
    const std::string& name() const;
    const std::string& value() const;

private:
    std::string name_;
    std::string value_;
};

} // namespace http
