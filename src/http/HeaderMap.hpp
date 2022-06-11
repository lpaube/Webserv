/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HeaderMap.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:48:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/11 16:25:25 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Header.hpp"
#include <map>
#include <string>

#define CONTENT_LENGTH_HEADER "content-length"
#define TRANSFER_ENCODING_HEADER "transfer-encoding"

namespace http
{
class HeaderMap
{
public:
    typedef std::map<std::string, std::string>::const_iterator const_iterator;

public:
    void add(const Header& header);
    const_iterator get(std::string name) const;
    const_iterator begin() const;
    const_iterator end() const;

private:
    std::map<std::string, std::string> headers_;
};

} // namespace http
