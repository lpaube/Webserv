#include "event/ConnectionEvent.hpp"

namespace event
{
ConnectionEvent::ConnectionEvent(const Socket* connection, bool read)
    : Event(connection), is_read_(read)
{
}

EventType ConnectionEvent::type() const
{
    return CONNECTION_EVENT;
}

bool ConnectionEvent::is_read() const
{
    return is_read_;
}

} // namespace event
