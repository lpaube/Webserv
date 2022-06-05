/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:40:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 19:17:19 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"

enum EventType
{
    TCP_STREAM_EVENT,
    CONNECTION_EVENT
};

class Event
{
public:
    Event(const Socket* socket, bool read);
    virtual ~Event();

public:
    virtual EventType type() const = 0;

public:
    const Socket* data() const;
    bool is_read() const;

protected:
    const Socket* socket_;
    bool is_read_;
};
