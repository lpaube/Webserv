#include "Event.hpp"

Event::Event(const Socket* socket, bool read) : socket_(socket), is_read_(read)
{
}

Event::~Event()
{
}

const Socket* Event::data() const
{
    return socket_;
}

bool Event::is_read() const
{
    return is_read_;
}
