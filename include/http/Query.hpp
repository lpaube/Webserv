/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Query.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 17:24:45 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:18:40 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

namespace http
{
class Query
{
public:
    Query(const std::string& str);

public:
    const std::string& name() const;
    const std::string& value() const;

private:
    std::string name_;
    std::string value_;
};

} // namespace http
