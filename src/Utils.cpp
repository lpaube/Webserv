/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <louis-philippe.aube@hotmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:40:28 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/20 15:18:57 by laube            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <algorithm>
#include <ctype.h>
#include <iostream>

bool vectorize_content(std::vector<std::string>& content, std::string& content_str)
{
    std::string::iterator start = content_str.begin();
    std::string::iterator end = content_str.end();
    std::string::iterator current = start;

    while (start != end) {
        while (*start > 0 && *start < 33)
            start++;
        current = start;
        if (start != end) {
            while (*current != ';') {
                if (current == end)
                    return false;
                current++;
            }
            std::string add(start, current);
            content.push_back(trim(add, WHITESPACE));
            start = current + 1;
        }
    }
    return true;
}

std::string trim(const std::string& str, const std::string& sep)
{
    std::string::const_iterator start = str.begin();
    while (start != str.end() && sep.find(*start) != std::string::npos) {
        ++start;
    }

    std::string::const_iterator end = str.end();
    while (end != start && sep.find(*(end - 1)) != std::string::npos) {
        --end;
    }
    return std::string(start, end);
}

std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> arr;
    std::string::size_type p1 = 0;
    std::string::size_type p2 = str.find(sep);

    while (p2 != std::string::npos) {
        if (p2 - p1 != 0) {
            arr.push_back(str.substr(p1, p2 - p1));
        }
        p1 = p2 + 1;
        p2 = str.find(sep, p1);
    }
    if (p1 < str.length()) {
        arr.push_back(str.substr(p1));
    }
    return arr;
}

std::string to_lower(const std::string& str)
{
    std::string ret = str;
    for (std::string::iterator it = ret.begin(); it != ret.end(); ++it) {
        *it = tolower(*it);
    }
    return ret;
}

std::vector<char>::const_iterator find_bytes(const std::vector<char>& data, const char* needle,
                                             size_t needle_size)
{
    if (data.size() < needle_size) {
        return data.end();
    }

    size_t s = data.size() - needle_size + 1;
    for (size_t i = 0; i < s; ++i) {
        if (memcmp(data.data() + i, needle, needle_size) == 0) {
            return data.begin() + (std::ptrdiff_t)i;
        }
    }
    return data.end();
}

std::vector<char>::iterator find_bytes(std::vector<char>& data, const char* needle,
                                       size_t needle_size)
{
    if (data.size() < needle_size) {
        return data.end();
    }

    size_t s = data.size() - needle_size + 1;
    for (size_t i = 0; i < s; ++i) {
        if (memcmp(data.data() + i, needle, needle_size) == 0) {
            return data.begin() + (std::ptrdiff_t)i;
        }
    }
    return data.end();
}

void print_bytes(const std::vector<char>& data)
{
    for (std::vector<char>::const_iterator it = data.begin(); it != data.end(); ++it) {
        std::cout << *it;
    }
    std::cout << std::endl;
}
