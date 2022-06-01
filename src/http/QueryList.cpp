/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryList.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:29:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:40:51 by mleblanc         ###   ########.fr       */
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
    while (query_str.find('&') != std::string::npos) {
        Query query = Query(get_next_word(query_str, "&"));
        values_.insert(std::make_pair(query.name(), query.value()));
    }
    Query query = Query(query_str);
    values_.insert(std::make_pair(query.name(), query.value()));
}

} // namespace http
