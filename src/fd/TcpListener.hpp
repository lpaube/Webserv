#pragma once

#include "FileDescriptor.hpp"
#include <netinet/in.h>
#include <string>

class TcpListener : public FileDescriptor
{
public:
    TcpListener(const std::string& address, uint16_t port, int backlog);

    virtual FDType type() const;

    const std::string& address_str() const;
    in_addr address() const;
    uint16_t port() const;

private:
    std::string addr_str_;
    in_addr addr_;
    uint16_t port_;
    int backlog_;
};
