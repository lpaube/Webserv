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
