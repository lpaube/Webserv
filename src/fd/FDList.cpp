/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:21:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 22:27:49 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "FDList.hpp"
#include <algorithm>

FDList::~FDList()
{
    close_all();
}

FDList::FDMap::mapped_type& FDList::operator[](const int fd)
{
    return fds_[fd];
}

void FDList::close_all()
{
    fds_.clear();
}

size_t FDList::erase(int fd)
{
    for (std::vector<pollfd>::iterator it = pfds_.begin(); it != pfds_.end(); ++it) {
        if (it->fd == fd) {
            pfds_.erase(it);
            break;
        }
    }
    return fds_.erase(fd);
}

std::pair<FDList::iterator, bool> FDList::insert(std::pair<int, SharedPtr<FileDescriptor> > value,
                                                 int events)
{
    std::pair<FDList::iterator, bool> ret = fds_.insert(value);

    if (!ret.second) {
        return ret;
    }

    pollfd pfd;
    pfd.fd = value.first;
    pfd.events = events;
    pfd.revents = 0;

    pfds_.insert(pfds_.begin(), pfd);

    return ret;
}

pollfd* FDList::pfds()
{
    return pfds_.data();
}

size_t FDList::size()
{
    return pfds_.size();
}
