#include "FileDescriptor.hpp"
#include <unistd.h>

FileDescriptor::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

FileDescriptor::FileDescriptor()
    : fd_(-1),
      state_(S_READ)
{
}

FileDescriptor::~FileDescriptor()
{
    if (is_init()) {
        close(fd());
    }
}

int FileDescriptor::fd() const
{
    return fd_;
}

bool FileDescriptor::is_init() const
{
    return fd_ != -1;
}

FDState FileDescriptor::state() const
{
    return state_;
}

void FileDescriptor::set_state(FDState state)
{
    state_ = state;
}
