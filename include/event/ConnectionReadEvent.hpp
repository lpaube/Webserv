/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionReadEvent.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 19:00:53 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 02:29:51 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "Socket.hpp"

namespace event
{
class ConnectionReadEvent : public Event
{
public:
    ConnectionReadEvent(const Socket* connection);

public:
    virtual EventType type() const;
};

} // namespace event
