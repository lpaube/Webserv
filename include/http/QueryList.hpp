/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   QueryList.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:12:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:36:19 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

namespace http
{
class QueryList
{
public:
    QueryList();
    QueryList(std::string query_str);

private:
    std::map<std::string, std::string> params_;
};

} // namespace http
