/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 22:19:12 by mafortin         ###   ########.fr       */
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

	while(start != end){
		start = findServerSep(this->file_content);
	}
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

ConfigParser::~ConfigParser(){}

std::string::iterator	findServerSep(std::string& content){
	std::size_t i = content.find("server");
	(void)i;
	return content.begin();
}
