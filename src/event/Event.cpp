#include "event/Event.hpp"

namespace event
{
Event::Event(const Socket* socket) : socket_(socket)
{
}

Event::~Event()
{
}

const Socket* Event::data() const
{
    return socket_;
}
} // namespace event
