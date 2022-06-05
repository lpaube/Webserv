/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWriteEvent.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 19:00:53 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 02:29:58 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "Socket.hpp"

namespace event
{
class ConnectionWriteEvent : public Event
{
public:
    ConnectionWriteEvent(const Socket* connection);

public:
    virtual EventType type() const;
};

} // namespace event
