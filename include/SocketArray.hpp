/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketArray.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/04 20:07:13 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 01:15:29 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Socket.hpp"
#include <vector>

class SocketArray
{
public:
    typedef std::vector<Socket*>::iterator iterator;

public:
    ~SocketArray();

public:
    void add(Socket* socket);
    iterator find(int fd);
    void erase(iterator pos);
    void clear();
    iterator begin();
    iterator end();

private:
    std::vector<Socket*> sockets_;
};
