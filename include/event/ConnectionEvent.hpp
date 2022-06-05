/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionEvent.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 19:00:53 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 20:33:38 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Connection.hpp"
#include "Event.hpp"

class ConnectionEvent : public Event
{
public:
    ConnectionEvent(const Connection& connection, bool read);

public:
    virtual EventType type() const;
};
