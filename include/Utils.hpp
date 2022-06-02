/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:37:34 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 17:21:07 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#define WHITESPACE "\t\n\v\f\r "

std::string trim(const std::string& str, const std::string& sep);
std::string get_next_word(std::string& str, const std::string& sep);
void to_lower(std::string& str);
bool vectorize_content(std::vector<std::string>& content, std::string& content_str);
