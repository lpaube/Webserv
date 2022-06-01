/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 16:38:07 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:00:16 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace http
{
class Header
{
public:
    Header(const std::string& str);
    Header(const std::string& n, const std::string& v);

public:
    const std::string& name() const;
    const std::string& value() const;

private:
    std::string name_;
    std::string value_;
};

} // namespace http
