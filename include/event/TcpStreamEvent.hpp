/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStreamEvent.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:57:02 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:39:37 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "Socket.hpp"

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
