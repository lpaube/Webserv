/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStreamEvent.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:00 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:40:01 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/TcpStreamEvent.hpp"

namespace event
{
TcpStreamEvent::TcpStreamEvent(Socket* stream)
    : Event(stream)
{
}

EventType TcpStreamEvent::type() const
{
    return TCP_STREAM_EVENT;
}
} // namespace event
