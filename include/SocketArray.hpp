/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketArray.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 16:54:28 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 18:26:11 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include <cstdint>
#include <netinet/in.h>
#include <poll.h>
#include <vector>

struct BindInfo {
    int fd;
    in_port_t port; // port in network byte order
    in_addr address;
};

class SocketArray
{
public:

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    ~SocketArray();

public:
    typedef std::vector<BindInfo>::const_iterator const_iterator;
    typedef std::vector<pollfd>::const_iterator poll_iterator;

public:
    void add(uint16_t port, in_addr address);
    const_iterator find_match(int fd) const;
    const_iterator find_match(uint16_t port, in_addr address) const;
    const_iterator begin() const;
    const_iterator end() const;
    pollfd* pfd_array();
    std::vector<pollfd>::size_type pfd_size();
    poll_iterator pfd_begin();
    poll_iterator pfd_end();

private:
    std::vector<pollfd> pollfds_;
    std::vector<BindInfo> binds_;
};
