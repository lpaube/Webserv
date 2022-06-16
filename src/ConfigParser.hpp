/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/16 14:13:31 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerParser.hpp"
#include <map>
#include <string>
#include <vector>

class ConfigParser
{
public:
    ConfigParser(const std::string& config_file);
    ~ConfigParser();
    unsigned int nbServer() const;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
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
