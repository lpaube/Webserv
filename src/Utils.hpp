/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:37:34 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/17 22:33:49 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#define WHITESPACE "\t\n\v\f\r "

std::string trim(const std::string& str, const std::string& sep);
std::vector<std::string> split(const std::string& str, char sep);
std::string to_lower(const std::string& str);
bool vectorize_content(std::vector<std::string>& content, std::string& content_str);
std::vector<char>::const_iterator find_bytes(const std::vector<char>& data, const char* needle,
                                             size_t needle_size);
std::vector<char>::iterator find_bytes(std::vector<char>& data, const char* needle,
                                       size_t needle_size);
void print_bytes(const std::vector<char>& data);

class SharedPtrBase
{
protected:
    SharedPtrBase()
        : ref_count_(NULL)
    {
    }

    SharedPtrBase(const SharedPtrBase& other)
        : ref_count_(other.ref_count_)
    {
    }

protected:
    std::size_t* ref_count_;
};

template <typename T>
class SharedPtr : public SharedPtrBase

{
public:
    SharedPtr()
        : SharedPtrBase(),
          ptr_(NULL)
    {
    }

    explicit SharedPtr(T* ptr)
        : SharedPtrBase(),
          ptr_(ptr)
    {
        ref_count_ = new std::size_t;
        *ref_count_ = 1;
    }

    SharedPtr(const SharedPtr& other)
        : SharedPtrBase(other),
          ptr_(other.ptr_)
    {
        ++(*ref_count_);
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other, T* ptr)
        : SharedPtrBase(other),
          ptr_(ptr)
    {
        ++(*ref_count_);
    }

    SharedPtr& operator=(SharedPtr rhs)
    {
        std::swap(ptr_, rhs.ptr_);
        std::swap(ref_count_, rhs.ref_count_);

        return *this;
    }

    ~SharedPtr()
    {
        release();
    }

    T* operator->() const
    {
        return ptr_;
    }

    T& operator*() const
    {
        return *ptr_;
    }

    void reset(T* ptr)
    {
        release();
        ptr_ = ptr;
        ref_count_ = new std::size_t;
        *ref_count_ = 1;
    }

    void reset()
    {
        release();
    }

    T* get() const
    {
        return ptr_;
    }

private:
    void release()
    {
        if (ref_count_ != NULL) {
            --(*ref_count_);
            if (*ref_count_ == 0) {
                delete ptr_;
                delete ref_count_;
            }
        }
        ptr_ = NULL;
        ref_count_ = NULL;
    }

private:
    T* ptr_;
};

template <typename T, typename U>
SharedPtr<T> static_pointer_cast(const SharedPtr<U>& other)
{
    return SharedPtr<T>(other, static_cast<T*>(other.get()));
}
