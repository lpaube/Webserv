/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:40:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:44:44 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "sock/Socket.hpp"

namespace event
{
enum EventType {
    TCP_STREAM_EVENT,
    CONNECTION_READ_EVENT,
    CONNECTION_WRITE_EVENT
};

class Event
{
public:
    Event(sock::Socket* socket);
    virtual ~Event();

public:
    virtual EventType type() const = 0;

public:
    sock::Socket* data();

protected:
    sock::Socket* socket_;
};
} // namespace event
