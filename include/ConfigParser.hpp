/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/01 14:46:35 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include <vector>
#include <map>
#include <ServerParser.hpp>

class ConfigParser
{
public:
	ConfigParser(std::string config_file);
	~ConfigParser();
	class ConfigFileException : public std::exception{
		public:
			virtual const char* what() const throw();
	};
	class ConfigSyntaxException : public std::exception{
		public:
			virtual const char* what() const throw();
	};
private:
	bool						min_server;
	std::string					getContent(std::fstream& file);
	std::string					file_content;
	void						createConfig();
	unsigned int				nb_server;
	void findServerStart(std::string::iterator& start);
	std::string::iterator findServerEnd(std::string::iterator start, std::string::iterator end);

public:
	std::vector<ServerParser>	serverparser;
};
