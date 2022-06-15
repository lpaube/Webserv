/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:40:28 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 15:00:05 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <algorithm>
#include <ctype.h>
#include <string>
#include <unistd.h>
#include <vector>

void putstr_fd(std::vector<char> msg, std::size_t fd)
{
    std::vector<char>::iterator it = msg.begin();
    std::vector<char>::iterator end = msg.end();
    while (it != end) {
        write(fd, &*it, 1);
        it++;
    }
}

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

std::string get_next_word(Buffer& buf, const char* sep, size_t sep_size)
{
    const char* ptr = buf.find(sep, sep_size);
    if (ptr == NULL) {
        return "";
    }
    std::string word(buf.cursor(), ptr);
    buf.advance_cursor((size_t)(ptr - buf.cursor()) + sep_size);
    return word;
}

std::string get_next_word(std::string& str, const std::string& sep)
{
    std::string::size_type pos = str.find(sep);
    if (pos == std::string::npos) {
        return "";
    }
    std::string word = str.substr(0, pos);
    str.erase(0, pos + sep.length());
    return word;
}

std::vector<std::string> split(const std::string& str, char sep)
{
    std::vector<std::string> arr;
    std::string::size_type p1 = 0;
    std::string::size_type p2 = str.find(sep);

    while (p2 != std::string::npos)
    {
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

void to_lower(std::string& str)
{
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        *it = tolower(*it);
    }
}
