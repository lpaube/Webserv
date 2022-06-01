/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderMap.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:48:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 02:05:52 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include <map>
#include <string>

#define CONTENT_LENGTH_HEADER "content-length"

namespace http
{
class HeaderMap
{
public:
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

public:
    void add(const Header& header);
    const_iterator get(const std::string& name) const;
    const_iterator begin() const;
    const_iterator end() const;

private:
    std::map<std::string, std::string> headers_;
};

} // namespace http
