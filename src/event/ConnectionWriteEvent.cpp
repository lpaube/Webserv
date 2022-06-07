/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWriteEvent.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 02:30:39 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:32 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/ConnectionWriteEvent.hpp"

namespace event
{
ConnectionWriteEvent::ConnectionWriteEvent(sock::Socket* connection)
    : Event(connection)
{
}

EventType ConnectionWriteEvent::type() const
{
    return CONNECTION_WRITE_EVENT;
}

} // namespace event
