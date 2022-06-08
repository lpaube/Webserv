#include "Socket.hpp"
#include <unistd.h>

namespace sock
{
Socket::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

Socket::Socket()
    : is_init_(false),
      read_(true)
{
}

Socket::~Socket()
{
    if (is_init()) {
        close(fd());
    }
}

void Socket::check_init() const
{
    if (!is_init()) {
        throw Exception("Must initialize TcpStream before using it");
    }
}

int Socket::fd() const
{
    return fd_;
}

bool Socket::is_init() const
{
    return is_init_;
}

bool Socket::read() const
{
    return read_;
}

void Socket::set_write()
{
    read_ = false;
}
} // namespace sock
