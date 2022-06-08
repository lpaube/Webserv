/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:37:34 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 00:32:23 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "sock/Buffer.hpp"
#include <cerrno>
#include <cstring>
#include <string>
#include <vector>

#define WHITESPACE "\t\n\v\f\r "

std::string trim(const std::string& str, const std::string& sep);
std::string get_next_word(sock::Buffer& buf, const char* sep, size_t sep_size);
std::string get_next_word(std::string& str, const std::string& sep);
void to_lower(std::string& str);
bool vectorize_content(std::vector<std::string>& content, std::string& content_str);
void putstr_fd(std::string& msg, std::size_t fd);

template <typename Exception>
void exception_errno(std::string msg)
{
    msg.append(strerror(errno));
    throw Exception(msg.c_str());
}
