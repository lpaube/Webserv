/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderList.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:48:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:04:18 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include <map>
#include <string>

namespace http
{
class HeaderList
{
public:
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

public:
    void add(const Header& header);
    HeaderList::const_iterator get(const std::string& name) const;

private:
    std::map<std::string, std::string> headers_;
};

} // namespace http
