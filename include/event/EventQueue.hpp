/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EventQueue.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 19:05:05 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 19:12:52 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Event.hpp"
#include <queue>

class EventQueue
{
public:
    typedef std::queue<Event*>::reference reference;

public:
    ~EventQueue();

public:
    reference front();
    void pop();
    void push(Event* event);

private:
    std::queue<Event*> events_;
};
