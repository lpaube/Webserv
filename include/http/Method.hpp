/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Method.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 15:23:54 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:18:59 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace http
{
enum Method
{
    BAD_METHOD,
    GET,
    POST,
    DELETE
};

const char* method_str(Method method);
Method method_from_str(const std::string& str);

} // namespace http
