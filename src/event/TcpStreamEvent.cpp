/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStreamEvent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:52 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/TcpStreamEvent.hpp"

namespace event
{
TcpStreamEvent::TcpStreamEvent(sock::Socket* stream)
    : Event(stream)
{
}

EventType TcpStreamEvent::type() const
{
    return TCP_STREAM_EVENT;
}
} // namespace event
