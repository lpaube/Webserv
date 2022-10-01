#pragma once

#include "FileDescriptor.hpp"
#include "Utils.hpp"
#include <cstddef>
#include <map>
#include <poll.h>
#include <vector>

class FDList
{
public:
    typedef std::map<int, SharedPtr<FileDescriptor> > FDMap;
    typedef FDMap::iterator iterator;

public:
    ~FDList();

    FDMap::mapped_type& operator[](const int fd);

public:
    size_t erase(int fd);
    std::pair<FDList::iterator, bool> insert(std::pair<int, SharedPtr<FileDescriptor> > value,
                                             int events);
    pollfd* pfds();
    size_t size();
    void close_all();

private:
    FDMap fds_;
    std::vector<pollfd> pfds_;
};
