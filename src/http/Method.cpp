/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Method.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 17:18:42 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/09 19:00:50 by mafortin         ###   ########.fr       */
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
		case OPTIONS:
			return "OPTIONS";
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
	  if (method_str(OPTIONS) == str) {
        return OPTIONS;
    }
    return BAD_METHOD;
}
} // namespace http
