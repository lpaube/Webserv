#pragma once

#include <exception>
#include <string>

class ExceptionBase : public std::exception
{
public:
    static const std::size_t MSG_SIZE = 512;

public:
    ExceptionBase(const std::string& msg);

    virtual const char* what() const throw();

private:
    char msg_[MSG_SIZE];
};
