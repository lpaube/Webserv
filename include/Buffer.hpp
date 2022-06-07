/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/06 15:02:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/06 19:51:00 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

class Buffer
{
public:
    Buffer(size_t size);

public:
    template <typename Iter>
    void append(Iter first, Iter last)
    {
        size_t count = (size_t)std::distance(first, last);
        if (space_left() < count) {
            last = first + count;
        }
        data_.insert(data_.end(), first, last);
    }

    const char* data() const;
    const char* find(const char* sep, size_t sep_size) const;
    size_t size() const;
    size_t capacity() const;
    size_t space_left() const;
    const char* cursor() const;
    void advance_cursor(size_t count);
    void erase_to_cursor();

private:
    const size_t size_;
    std::vector<char> data_;
    const char* cursor_;
};
