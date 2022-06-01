/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryMap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:29:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 01:27:37 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/QueryMap.hpp"
#include "Utils.hpp"
#include "http/QueryParam.hpp"

namespace http
{
QueryMap::QueryMap()
{
}

QueryMap::QueryMap(std::string query_str)
{
    if (query_str.empty()) {
        return;
    }

    while (query_str.find('&') != std::string::npos) {
        QueryParam param = QueryParam(get_next_word(query_str, "&"));
        params_[param.name()] = param.value();
    }
    QueryParam param = QueryParam(query_str);
    params_[param.name()] = param.value();
}

QueryMap::const_iterator QueryMap::begin() const
{
    return params_.begin();
}

QueryMap::const_iterator QueryMap::end() const
{
    return params_.end();
}

} // namespace http
