/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/08 13:51:23 by mafortin         ###   ########.fr       */
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
    ConfigParser(std::string config_file);
    ~ConfigParser();
	unsigned int nbServer() const;
    class ConfigFileException : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

    class ConfigSyntaxException : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

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
