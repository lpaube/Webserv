/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 12:34:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 12:42:23 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Connection.hpp"

namespace http
{
void parse_request_line(Connection& c);
void parse_headers(Connection& c);
void parse_body(Connection& c);

} // namespace http
