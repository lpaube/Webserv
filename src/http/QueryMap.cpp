/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryMap.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:29:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 00:31:06 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/QueryMap.hpp"
#include "http/Query.hpp"
#include "Utils.hpp"

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
        Query query = Query(get_next_word(query_str, "&"));
        params_.insert(std::make_pair(query.name(), query.value()));
    }
    Query query = Query(query_str);
    params_.insert(std::make_pair(query.name(), query.value()));
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
