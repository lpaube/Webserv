/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListenerEvent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:52 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/TcpListenerEvent.hpp"

namespace event
{
TcpListenerEvent::TcpListenerEvent(sock::Socket* stream)
    : Event(stream)
{
}

EventType TcpListenerEvent::type() const
{
    return TCP_LISTENER_EVENT;
}
} // namespace event
