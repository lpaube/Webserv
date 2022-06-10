/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/06 15:02:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/10 15:16:28 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>
#include "ExceptionBase.hpp"

class Buffer
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg);
    };

public:
    Buffer(size_t size);
    Buffer& operator=(const Buffer& other);

public:
    template <typename Iter>
    void append(Iter first, Iter last)
    {
        size_t count = (size_t)std::distance(first, last);
        if (space_left() < count) {
            throw Exception("Buffer is full");
        }
        data_.insert(data_.end(), first, last);
    }
    void append(char c);

    const char* data() const;
    const char* find(const char* sep, size_t sep_size) const;
    size_t size() const;
    size_t capacity() const;
    size_t space_left() const;
    size_t cursor_size() const;
    const char* cursor() const;
    void advance_cursor(size_t count);
    void erase_to_cursor();
    void clear();

private:
    size_t capacity_;
    std::vector<char> data_;
    const char* cursor_;
};
