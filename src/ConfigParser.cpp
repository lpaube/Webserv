/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/18 15:18:37 by mafortin         ###   ########.fr       */
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

ConfigParser::ConfigParser(const std::string& config_file)
{
    std::fstream file;
    min_server_ = false;
    nb_server_ = 0;
    file.open(config_file.c_str(), std::fstream::in);
    if (file.is_open() == false)
        throw Exception("Error: Config file\n");
    this->file_content_ = get_content(file);
    file.close();
    create_config();
}

void ConfigParser::create_config()
{
    std::string::iterator start;
    std::string::iterator server_end;

    while (start != this->file_content_.end()) {
        start = this->file_content_.begin();
        find_server_start(start);
        if (min_server_ == false) {
            throw Exception("Error: Config, no server found\n");
        }
        if (start == this->file_content_.end())
            break;
        server_end = find_server_end(start, this->file_content_.end());
        ServerParser add(start, server_end);
        serverparser.push_back(add);
        start = server_end + 2;
        std::string swap(start, this->file_content_.end());
        this->file_content_.assign(swap);
        min_server_ = true;
        nb_server_++;
    }
}

std::string::iterator ConfigParser::find_server_end(std::string::iterator start,
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

void ConfigParser::find_server_start(std::string::iterator& start)
{
    std::size_t i = this->file_content_.find("server");
    if (i == std::string::npos) {
        start = file_content_.end();
        return;
    }
    i += 6;
    min_server_ = true;
    start += static_cast<long>(i);
    while (*start == ' ') {
        start++;
    }
    if (*start != '{') {
        throw Exception("Error: Config, no opening { found after server declaration\n");
    } else
        start++;
}

std::string ConfigParser::get_content(std::fstream& file)
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

unsigned int ConfigParser::nbserver() const
{
    return this->nb_server_;
}

ConfigParser::~ConfigParser()
{
}
