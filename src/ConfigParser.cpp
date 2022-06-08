/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/08 00:30:41 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include <exception>
#include <fstream>
#include <iostream>
#include <string.h>
#include <string>

ConfigParser::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

ConfigParser::ConfigParser(std::string config_file)
{
    std::fstream file;
    min_server = false;
    nb_server = 0;
    file.open(config_file.c_str(), std::fstream::in);
    if (file.is_open() == false)
        throw Exception("Error: Config file\n");
    this->file_content = getContent(file);
    file.close();
    createConfig();
}

void ConfigParser::createConfig()
{
    std::string::iterator start;
    std::string::iterator server_end;
    std::string::iterator end;

    while (start != this->file_content.end()) {
        start = this->file_content.begin();
        findServerStart(start);
        if (min_server == false) {
            throw Exception("Error: Config, no server found\n");
        }
        if (start == this->file_content.end())
            break;
        server_end = findServerEnd(start, this->file_content.end());
        ServerParser add(start, server_end);
        serverparser.push_back(add);
        start = server_end + 2;
        std::string swap(start, this->file_content.end());
        this->file_content.assign(swap);
        min_server = true;
        nb_server++;
    }
}

std::string::iterator ConfigParser::findServerEnd(std::string::iterator start,
                                                  std::string::iterator end)
{
    bool open = false;
    while (start != end) {
        if (*start == '{' && open == true)
            throw Exception("Error: Config, {{}} scopeception\n");
        if (*start == '{')
            open = true;
        else if (*start == '}' && open == true)
            open = false;
        else if (*start == '}')
            return start - 1;
        start++;
    }
    throw Exception("Error: Config, {} scope no closed\n");
    return end;
}

void ConfigParser::findServerStart(std::string::iterator& start)
{
    std::size_t i = this->file_content.find("server");
    if (i == std::string::npos) {
        start = file_content.end();
        return;
    }
    i += 6;
    min_server = true;
    start += static_cast<long>(i);
    while (*start == ' ') {
        start++;
    }
    if (*start != '{') {
        throw Exception("Error: Config, no opening { found after server declaration\n");
    } else
        start++;
}

std::string ConfigParser::getContent(std::fstream& file)
{
    file.seekg(0, file.end);
    long len = file.tellg();
    file.seekg(0, file.beg);
    char* buf = new char[len + 1];
    file.read(buf, len);
    file.close();
    buf[len] = 0;
    std::string ret(buf);
    delete[] buf;
    return ret;
}

ConfigParser::~ConfigParser()
{
}
