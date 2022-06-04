#include "Socket.hpp"
#include <unistd.h>

Socket::Exception::Exception(const char* msg) : ExceptionBase(msg)
{
}

Socket::Socket() : is_init_(false)
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
