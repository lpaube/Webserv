/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:08:15 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/28 19:44:27 by mafortin         ###   ########.fr       */
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
    : min_server_(false),
      file_content_(),
      nb_server_(0)
{
    std::fstream file;
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

    start = this->file_content_.begin();
    while (start != this->file_content_.end()) {
        find_server_start(start);
        if (min_server_ == false) {
            throw Exception("Error: Config, no server found\n");
        }
        if (start == this->file_content_.end())
            break;
        server_end = find_server_end(start, this->file_content_.end());
        ServerParser add(start, server_end);
        serverparser.push_back(add);
        start = server_end + 1;
        this->file_content_.assign(start, this->file_content_.end());
        min_server_ = true;
        nb_server_++;
        start = this->file_content_.begin();
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
            return start;
        start++;
    }
    throw Exception("Error: Config, {} scope not closed\n");
    return end;
}

void ConfigParser::find_server_start(std::string::iterator& start)
{
    std::size_t i = 0;
    while (isspace(this->file_content_[i])) {
        i++;
        if (this->file_content_[i] == 's')
            break;
        if (i == std::string::npos || !this->file_content_[i]) {
            start = file_content_.end();
            return;
        }
        if (!isspace(this->file_content_[i])) {
            throw Exception("Error config, wrong character out of server scope{}\n");
        }
    }
    this->file_content_.erase(0, i);
    start = this->file_content_.begin();
    if (start == file_content_.end())
        return;
    if (this->file_content_.compare(0, 6, "server") != 0) {
        throw Exception("Error config, wrong character out of server scope{}\n");
    }
    min_server_ = true;
    start += 6;
    while (isspace(*start)) {
        start++;
    }
    if (*start != '{') {
        throw Exception("Error config, no opening { found after server declaration\n");
    } else
        start++;
}

std::string ConfigParser::get_content(std::fstream& file)
{
    return std::string(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
}

unsigned int ConfigParser::nbserver() const
{
    return this->nb_server_;
}

ConfigParser::~ConfigParser()
{
}
