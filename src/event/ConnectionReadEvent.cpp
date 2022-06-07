/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionReadEvent.cpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 02:30:39 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:39:52 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/ConnectionReadEvent.hpp"

namespace event
{
ConnectionReadEvent::ConnectionReadEvent(Socket* connection)
    : Event(connection)
{
}

EventType ConnectionReadEvent::type() const
{
    return CONNECTION_READ_EVENT;
}

} // namespace event
