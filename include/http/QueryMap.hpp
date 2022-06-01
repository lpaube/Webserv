/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryList.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:12:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 00:30:21 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

namespace http
{
class QueryMap
{
public:
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

public:
    QueryMap();
    QueryMap(std::string query_str);

public:
    const_iterator begin() const;
    const_iterator end() const;

private:
    std::map<std::string, std::string> params_;
};

} // namespace http
