#include "ConnectionEvent.hpp"

ConnectionEvent::ConnectionEvent(const Connection& connection, bool read)
    : Event(static_cast<const Socket*>(&connection), read)
{
}

EventType ConnectionEvent::type() const
{
    return CONNECTION_EVENT;
}
