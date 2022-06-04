/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/03 22:44:04 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 23:54:08 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ExceptionBase.hpp"
#include <sys/socket.h>
#include <vector>

class Client
{
public:
    typedef std::vector<char>::const_iterator data_iter;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Client();
    ~Client();

public:
    bool operator==(int fd) const;

public:
    void accept(int server_fd, timeval timeout);
    int fd() const;
    data_iter data_begin() const;
    data_iter data_end() const;
    template <typename Iter>
    void append_data(Iter first, Iter last)
    {
        data_.insert(data_.end(), first, last);
    }

private:
    int fd_;
    sockaddr addr_;
    socklen_t addrlen_;
    bool is_init_;
    std::vector<char> data_;
};
