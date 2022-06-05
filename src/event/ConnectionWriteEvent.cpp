/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWriteEvent.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 02:30:39 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 02:31:16 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/ConnectionWriteEvent.hpp"

namespace event
{
ConnectionWriteEvent::ConnectionWriteEvent(const Socket* connection) : Event(connection)
{
}

EventType ConnectionWriteEvent::type() const
{
    return CONNECTION_WRITE_EVENT;
}

} // namespace event
