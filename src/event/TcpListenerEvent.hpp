/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpListenerEvent.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:57:02 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:59 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "sock/Socket.hpp"

namespace event
{
class TcpListenerEvent : public Event
{
public:
    TcpListenerEvent(sock::Socket* stream);

public:
    virtual EventType type() const;
};
} // namespace event
