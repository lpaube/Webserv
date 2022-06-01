/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderList.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:50:17 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:40:27 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/HeaderList.hpp"

namespace http
{
void HeaderList::add(const Header& header)
{
    headers_.insert(std::make_pair(header.name(), header.value()));
}

HeaderList::const_iterator HeaderList::get(const std::string& name) const
{
    return headers_.find(name);
}
} // namespace http
