/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/18 15:18:26 by mafortin         ###   ########.fr       */
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
    unsigned int nbserver() const;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

private:
    std::string get_content(std::fstream& file);
    void create_config();
    void find_server_start(std::string::iterator& start);
    std::string::iterator find_server_end(std::string::iterator start, std::string::iterator end);

private:
    bool min_server_;
    std::string file_content_;
    unsigned int nb_server_;

public:
    std::vector<ServerParser> serverparser;
};
