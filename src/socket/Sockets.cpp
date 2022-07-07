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

#include "Sockets.hpp"
#include <algorithm>

Sockets::~Sockets()
{
    close_all();
}

Sockets::SocketMap::mapped_type& Sockets::operator[](const int fd)
{
    return sockets_[fd];
}

void Sockets::close_all()
{
    for (iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        delete it->second;
    }
    sockets_.clear();
}

size_t Sockets::erase(int fd)
{
    for (std::vector<pollfd>::iterator it = pfds_.begin(); it != pfds_.end(); ++it) {
        if (it->fd == fd) {
            pfds_.erase(it);
            break;
        }
    }
    return sockets_.erase(fd);
}

std::pair<Sockets::iterator, bool> Sockets::insert(std::pair<int, Socket*> value, int events)
{
    std::pair<Sockets::iterator, bool> ret = sockets_.insert(value);

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

pollfd* Sockets::pfds()
{
    return pfds_.data();
}

size_t Sockets::size()
{
    return pfds_.size();
}
