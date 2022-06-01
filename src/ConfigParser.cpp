/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 21:07:31 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"
#include <fstream>
#include <iostream>
#include <string>


ConfigParser::ConfigParser(std::string config_file){
	std::fstream	file;
	(void)this->config_content;
	file.open(config_file, std::fstream::in);
	 if (file.is_open() == false)
		throw ConfigFileException();
	this->file_content = getContent(file);
	file.close();
	createConfig();
}

void	ConfigParser::createConfig()[
	while(true){
		
	}
]

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
