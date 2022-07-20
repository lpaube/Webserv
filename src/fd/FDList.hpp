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
