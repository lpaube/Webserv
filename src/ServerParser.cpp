/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:47:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/01 15:44:19 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerParser.hpp"
#include "Utils.hpp"
#include <string>

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end) : str_content(beg, end){
	buildContent();
}

void	ServerParser::buildContent(){
	std::string::iterator start = str_content.begin();
	std::string::iterator end = str_content.end();
	std::string::iterator current = start;
	while (current != end){
		if (*current == ';'){
			std::string add(start, current);
			server_content.push_back(trim_white_spaces(add));
			start = current + 1;
			current = start;
		}
		else
			current++;
	}
}
