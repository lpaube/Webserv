#include "EventQueue.hpp"

EventQueue::~EventQueue()
{
    while (!events_.empty()) {
        pop();
    }
}

EventQueue::reference EventQueue::front()
{
    return events_.front();
}

void EventQueue::pop()
{
    delete events_.front();
    events_.pop();
}

void EventQueue::push(Event* event)
{
    events_.push(event);
}
