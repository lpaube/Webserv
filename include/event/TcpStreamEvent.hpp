/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TcpStreamEvent.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 18:57:02 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 19:29:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include "TcpStream.hpp"

class TcpStreamEvent : public Event
{
public:
    TcpStreamEvent(const TcpStream& stream, bool read);

public:
    virtual EventType type() const;
};
