/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:40:28 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 16:05:35 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"
#include <algorithm>
#include <ctype.h>
#include <vector>
#include <string>

bool	vectorize_content(std::vector<std::string>& content, std::string& content_str){
	std::string::iterator start = content_str.begin();
	std::string::iterator end = content_str.end();
	std::string::iterator current = start;

	while (start != end){
		while (*start > 0 && *start < 33)
			start++;
		current = start;
		if (start != end){
			while(*current != ';'){
				if (current == end)
					return false;
				current++;
			}
			std::string add(start, current);
			content.push_back(trim_white_spaces(add));
			start = current + 1;
			}
	}
	return true;
}

std::string	trim_white_spaces(std::string str){
	std::string::iterator start = str.begin();
	std::string::iterator end = str.end();

	while (*start > 0 && *start < 33)
		++start;
	while (*end > 0 && *end < 33)
		--end;
	std::string ret(start, end);
	return ret;
}

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
