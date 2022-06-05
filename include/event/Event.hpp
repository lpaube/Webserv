/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:40:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 02:28:57 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"

namespace event
{
enum EventType
{
    TCP_STREAM_EVENT,
    CONNECTION_READ_EVENT,
    CONNECTION_WRITE_EVENT
};

class Event
{
public:
    Event(const Socket* socket);
    virtual ~Event();

public:
    virtual EventType type() const = 0;

public:
    const Socket* data() const;

protected:
    const Socket* socket_;
};
} // namespace event
