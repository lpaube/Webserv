/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Sockets.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/12 19:15:38 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 22:44:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include <cstddef>
#include <map>
#include <poll.h>
#include <vector>

class Sockets
{
public:
    typedef std::map<int, Socket*> SocketMap;
    typedef SocketMap::iterator iterator;

public:
    ~Sockets();

    SocketMap::mapped_type& operator[](const int fd);

    void close_all();

public:
    size_t erase(int fd);
    std::pair<Sockets::iterator, bool> insert(std::pair<int, Socket*> value, int events);
    pollfd* pfds();
    size_t size();

private:
    SocketMap sockets_;
    std::vector<pollfd> pfds_;
};
