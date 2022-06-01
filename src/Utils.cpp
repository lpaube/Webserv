/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:40:28 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 15:31:19 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <algorithm>
#include <ctype.h>

std::string trim(const std::string& str, char sep)
{
    std::string::const_iterator start = str.begin();
    while (start != str.end() && *start == sep) {
        ++start;
    }

    std::string::const_iterator end = str.end();
    while (end != start && *(end - 1) == sep) {
        --end;
    }
    return std::string(start, end);
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

void to_lower(std::string& str)
{
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        *it = tolower(*it);
    }
}
