/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryList.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:29:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:36:18 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/QueryList.hpp"
#include "http/Query.hpp"
#include "Utils.hpp"

namespace http
{
QueryList::QueryList()
{
}

QueryList::QueryList(std::string query_str)
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

} // namespace http
