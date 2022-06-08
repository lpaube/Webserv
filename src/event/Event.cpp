/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:06 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:40 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/Event.hpp"

namespace event
{
Event::Event(sock::Socket* socket)
    : socket_(socket)
{
}

Event::~Event()
{
}

sock::Socket* Event::data()
{
    return socket_;
}
} // namespace event
