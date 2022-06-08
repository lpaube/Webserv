/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 12:34:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:47:29 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "sock/Connection.hpp"

namespace http
{
void parse_request_line(sock::Connection& c);
void parse_headers(sock::Connection& c);
void parse_body(sock::Connection& c);

} // namespace http
