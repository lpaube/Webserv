/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Event.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:06 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:40:07 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/Event.hpp"

namespace event
{
Event::Event(Socket* socket)
    : socket_(socket)
{
}

Event::~Event()
{
}

Socket* Event::data()
{
    return socket_;
}
} // namespace event
