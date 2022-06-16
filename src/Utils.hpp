/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:37:34 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 23:46:16 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

#define WHITESPACE "\t\n\v\f\r "

std::string trim(const std::string& str, const std::string& sep);
std::vector<std::string> split(const std::string& str, char sep);
std::string to_lower(const std::string& str);
bool vectorize_content(std::vector<std::string>& content, std::string& content_str);
