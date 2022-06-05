#include "SocketArray.hpp"
#include <algorithm>

void SocketArray::add(Socket* socket)
{
    sockets_.push_back(socket);
}

SocketArray::iterator SocketArray::find(int fd)
{
    for (iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        if ((*it)->fd() == fd) {
            return it;
        }
    }
    return sockets_.end();
}

void SocketArray::erase(iterator pos)
{
    delete *pos;
    sockets_.erase(pos);
}

void SocketArray::clear()
{
    for (iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        delete *it;
    }
    sockets_.clear();
}

SocketArray::iterator SocketArray::begin()
{
    return sockets_.begin();
}

SocketArray::iterator SocketArray::end()
{
    return sockets_.end();
}

SocketArray::~SocketArray()
{
    clear();
}
