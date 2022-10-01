#pragma once

#include "FileDescriptor.hpp"
#include <unistd.h>
#include <vector>

class File : public FileDescriptor
{
public:
    File(const std::string& filepath, FDState mode);

    virtual FDType type() const;

    const std::string& filepath() const;
    bool write_done() const;
    bool read_done() const;
    void append_write_data(const std::vector<char>& data);
    const std::vector<char>& get_read_data() const;
    int handle();
    bool error() const;

private:
    std::vector<char> write_buf_;
    std::vector<char> read_buf_;
    bool eof_;
    std::string filepath_;
    ssize_t w_index_;
    bool error_;
};
