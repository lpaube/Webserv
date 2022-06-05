#include "TcpStreamEvent.hpp"

TcpStreamEvent::TcpStreamEvent(const TcpStream& stream, bool read)
    : Event(static_cast<const Socket*>(&stream), read)
{
}

EventType TcpStreamEvent::type() const
{
    return TCP_STREAM_EVENT;
}
