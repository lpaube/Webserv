/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/01 10:49:12 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"
#include <fstream>
#include <iostream>
#include <string>

ConfigParser::ConfigParser(std::string config_file){
	std::fstream	file;
	(void)this->serverparser;
	file.open(config_file, std::fstream::in);
	 if (file.is_open() == false)
		throw ConfigFileException();
	this->file_content = getContent(file);
	file.close();
	createConfig();
}

void	ConfigParser::createConfig(){
	std::string::iterator start = this->file_content.begin();
	std::string::iterator end = this->file_content.end();
	std::string::iterator server_end;

	//while(start != end){
		findServerStart(start);
		server_end = findServerEnd(start, end);
		ServerParser add(start, server_end);
		serverparser.push_back(add);
		start = server_end;
	//}
}

std::string::iterator ConfigParser::findServerEnd(std::string::iterator start, std::string::iterator& end){
	bool	open = false;
	while (start != end){
		if (*start == '{')
			open = true;
		else if (*start == '}' && open == true)
			open = false;
		else if (*start == '}')
			return start - 1;
		start++;
	}
	throw ConfigSyntaxException();
	return end;
}

void ConfigParser::findServerStart(std::string::iterator& start){
	std::size_t	i =  this->file_content.find("server") + 6;
	start += static_cast<long>(i);
	while(*start == ' ')
			start++;
	if (*start != '{')
	{
		throw ConfigSyntaxException();
	}
	else
		start++;
}

std::string ConfigParser::getContent(std::fstream& file){
	file.seekg(0, file.end);
	long	len = file.tellg();
	file.seekg(0, file.beg);
	char	*buf = new char [len + 1];
	file.read(buf, len);
	file.close();
	buf[len] = 0;
	std::string ret(buf);
	delete[] buf;
	return ret;
}

const char* ConfigParser::ConfigFileException::what() const throw(){
				return ("Error: Config file\n");
}
const char* ConfigParser::ConfigSyntaxException::what() const throw(){
				return ("Error: Syntax error in config file\n");
}

ConfigParser::~ConfigParser(){}
