/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Method.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 15:23:54 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/09 19:00:10 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

namespace http
{
enum Method {
    BAD_METHOD,
    GET,
    POST,
    DELETE,
	OPTIONS
};

const char* method_str(Method method);
Method method_from_str(const std::string& str);

} // namespace http
