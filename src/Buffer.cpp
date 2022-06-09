/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Buffer.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/06 15:06:21 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/09 14:40:50 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Buffer.hpp"
#include <cstdlib>
#include <cstring>

Buffer::Exception::Exception(const std::string& msg)
    : ExceptionBase(msg)
{
}

Buffer::Buffer(size_t size)
    : capacity_(size)
{
    data_.reserve(capacity_);
    cursor_ = data_.data();
}

Buffer& Buffer::operator=(const Buffer& other)
{
    std::vector<char>::difference_type index = other.cursor() - other.data();
    capacity_ = other.capacity_;
    data_ = other.data_;
    cursor_ = data_.data() + index;
    return *this;
}

const char* Buffer::data() const
{
    return data_.data();
}

const char* Buffer::find(const char* sep, size_t sep_size) const
{
    const char* ptr = NULL;
    if ((ptr = (const char*)memchr(cursor(), *sep, space_left())) != NULL) {
        for (size_t i = 0; i < sep_size; ++i) {
            if (ptr[i] != sep[i]) {
                return NULL;
            }
        }
        return ptr;
    }
    return NULL;
}

size_t Buffer::size() const
{
    return data_.size();
}

size_t Buffer::capacity() const
{
    return capacity_;
}

size_t Buffer::space_left() const
{
    return (size_t)((data() + capacity()) - cursor_);
}

const char* Buffer::cursor() const
{
    return cursor_;
}

void Buffer::advance_cursor(size_t count)
{
    cursor_ += count;
}

void Buffer::erase_to_cursor()
{
    const std::vector<char>::difference_type index = cursor_ - data_.data();
    data_.erase(data_.begin(), data_.begin() + index);
    cursor_ = data_.data();
}

void Buffer::clear()
{
    data_.clear();
    cursor_ = data_.data();
}

void Buffer::append(char c)
{
    if (size() == capacity()) {
        throw Exception("Buffer is full");
    }
    data_.push_back(c);
}
