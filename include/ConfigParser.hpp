/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/03 11:43:34 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <ServerParser.hpp>
#include <map>
#include <string>
#include <vector>

class ConfigParser
{
public:
	class Exception : public ExceptionBase
	{
    	public:
        Exception(const char* msg);
    };
public:
    ConfigParser(std::string config_file);
    ~ConfigParser();
private:
    std::string getContent(std::fstream& file);
    void createConfig();
    void findServerStart(std::string::iterator& start);
    std::string::iterator findServerEnd(std::string::iterator start, std::string::iterator end);

private:
    bool min_server;
    std::string file_content;
    unsigned int nb_server;

public:
    std::vector<ServerParser> serverparser;
};
