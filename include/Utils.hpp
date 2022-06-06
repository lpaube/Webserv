/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:37:34 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/04 11:32:56 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#define WHITESPACE "\t\n\v\f\r "

std::string trim(const std::string& str, const std::string& sep);
std::string get_next_word(std::string& str, const std::string& sep);
void to_lower(std::string& str);
bool vectorize_content(std::vector<std::string>& content, std::string& content_str);
