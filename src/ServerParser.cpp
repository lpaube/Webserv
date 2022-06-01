/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:47:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/01 17:33:29 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerParser.hpp"
#include "Utils.hpp"
#include <string>
#include <iostream>
#include "ConfigParser.hpp"

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end) : str_content(beg, end){
	buildContent();
}

void	ServerParser::printContent() const{
	int size = server_content.size();
	std::cout << "PRINTING SERVER CONTENT\n"; 
	for(int i = 0; i < size; i++){
		std::cout << server_content[static_cast<unsigned long>(i)] << std::endl;
	}
}

std::size_t	ServerParser::findLocStart(std::size_t i){
	i = str_content.find("location");
	if (i == std::string::npos)
		return i;
	i += 8;
	while (str_content[i] > 0 && str_content[i] < 33)
		i++;
	if (str_content[i] != '{')
		throw SyntaxException();
	i++;
	return i;
}

std::size_t ServerParser::findLocEnd(std::size_t i, std::size_t end){
	while ()
}

void	ServerParser::buildLocation(){
	std::size_t i = 0;
	std::size_t	scope_end;
	std::size_t end = str_content.length();

	while (i != end){
		i = findLocStart(i);
		if (i == std::string::npos)
			return ;
		scope_end = findLocEnd(i, end);
	}



}

void	ServerParser::buildContent(){
	std::string::iterator start = str_content.begin();
	std::string::iterator end = str_content.end();
	std::string::iterator current = start;

	while (start != end){
		while (*start > 0 && *start < 33)
			start++;
		current = start;
		if (start != end){
			while(*current != ';'){
				if (current == end)
					throw NoSepException();
				current++;
			}
			std::string add(start, current);
			server_content.push_back(trim_white_spaces(add));
			start = current + 1;
			}
	}
}

const char* ServerParser::NoSepException::what() const throw(){
				return ("Error: No ';' found after value\n");
}

const char* ServerParser::SyntaxException::what() const throw(){
				return ("Error: Syntax error in config file\n");
}
