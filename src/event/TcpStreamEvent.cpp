#include "event/TcpStreamEvent.hpp"

namespace event
{
TcpStreamEvent::TcpStreamEvent(const Socket* stream) : Event(stream)
{
}

EventType TcpStreamEvent::type() const
{
    return TCP_STREAM_EVENT;
}
} // namespace event
