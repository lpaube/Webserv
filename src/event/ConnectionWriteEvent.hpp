/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionWriteEvent.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 19:00:53 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:36:37 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "socket/Socket.hpp"

namespace event
{
class ConnectionWriteEvent : public Event
{
public:
    ConnectionWriteEvent(Socket* connection);

public:
    virtual EventType type() const;
};

} // namespace event
