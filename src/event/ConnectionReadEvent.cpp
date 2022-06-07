/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionReadEvent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 02:30:39 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:18 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/ConnectionReadEvent.hpp"

namespace event
{
ConnectionReadEvent::ConnectionReadEvent(sock::Socket* connection)
    : Event(connection)
{
}

EventType ConnectionReadEvent::type() const
{
    return CONNECTION_READ_EVENT;
}

} // namespace event
