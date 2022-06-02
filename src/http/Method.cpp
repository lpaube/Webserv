/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 17:18:42 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:40:36 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Method.hpp"

namespace http
{
const char* method_str(Method method)
{
    switch (method) {
        case GET:
            return "GET";
        case POST:
            return "POST";
        case DELETE:
            return "DELETE";
        default:
            return "BAD METHOD";
    }
}

Method method_from_str(const std::string& str)
{
    if (method_str(GET) == str) {
        return GET;
    }
    if (method_str(POST) == str) {
        return POST;
    }
    if (method_str(DELETE) == str) {
        return DELETE;
    }
    return BAD_METHOD;
}
} // namespace http
