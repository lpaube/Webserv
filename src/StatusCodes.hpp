/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCodes.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:45:38 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/16 17:03:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <string>

/*
 * We can access the status code messages through a static method
 * like so: StatusCodes::getCodeMsg(code)
 * See StatusCodes.cpp for all the status codes
 */

namespace StatusCode
{
static std::map<int, const char*> codes;

std::string get_code_msg(int code);
} // namespace StatusCode
