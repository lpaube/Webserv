/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventQueue.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/05 06:40:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 06:40:09 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "event/EventQueue.hpp"

namespace event
{
EventQueue::~EventQueue()
{
    while (!empty()) {
        delete pop();
    }
}

EventQueue::reference EventQueue::front()
{
    return events_.front();
}

Event* EventQueue::pop()
{
    Event* ev = events_.front();
    events_.pop();
    return ev;
}

void EventQueue::push(Event* event)
{
    events_.push(event);
}

bool EventQueue::empty() const
{
    return events_.empty();
}

} // namespace event
