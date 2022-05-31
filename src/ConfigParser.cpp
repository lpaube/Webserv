/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configparser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 19:22:08 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"
#include <fstream>
#include <iostream>

ConfigParser::ConfigParser(std::string config_file){
	std::ofstream	file;
	file.open(config_file, std::ofstream::in);
	if (file.is_open() == false)
		throw ConfigFileException();

}

const char* ConfigParser::ConfigFileException::what() const throw(){
				return ("Error: Config file\n");
}
