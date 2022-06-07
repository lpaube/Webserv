/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStreamEvent.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:57:02 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:36:50 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "socket/Socket.hpp"

namespace event
{
class TcpStreamEvent : public Event
{
public:
    TcpStreamEvent(Socket* stream);

public:
    virtual EventType type() const;
};
} // namespace event
