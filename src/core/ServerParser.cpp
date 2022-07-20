/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:47:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/29 18:46:25 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerParser.hpp"
#include "ConfigParser.hpp"
#include "LocationContent.hpp"
#include "Utils.hpp"
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>

#define BYTES_IN_MB 1048576

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end)
    : server_content_(),
      location_(),
      str_content_(beg, end),
      config(),
      nb_location(0)
{
    build_location();
    for (unsigned long i = 0; i < static_cast<unsigned long>(this->nb_location); i++) {
        std::string line = this->location_[i].loc_content_str;
        vectorize_content(this->location_[i].loc_content, line);
    }
    if (vectorize_content(this->server_content_, str_content_) == false)
        throw NoSepException();
    parse_server_vars();
    parse_location_vars();
}

std::size_t ServerParser::find_loc_start(std::size_t i)
{
    i = str_content_.find("location");
    if (i == std::string::npos)
        return i;
    nb_location++;
    location_.resize(static_cast<unsigned long>(nb_location));
    i += 8;
    while (str_content_[i] > 0 && str_content_[i] < 33)
        i++;
    while (str_content_[i] && str_content_[i] != '{' && str_content_[i] >= 33) {
        location_[static_cast<unsigned long>(nb_location - 1)].path.push_back(str_content_[i]);
        i++;
    }
    while (str_content_[i] > 0 && str_content_[i] < 33)
        i++;
    if (str_content_[i] != '{')
        throw SyntaxException();
    i++;
    return i;
}

std::size_t ServerParser::find_loc_end(std::size_t i, std::size_t end)
{
    while (i != end) {
        if (this->str_content_[i] == '{')
            throw SyntaxException();
        if (this->str_content_[i] == '}')
            return i - 1;
        i++;
    }
    throw SyntaxException();
}

void ServerParser::build_location()
{
    std::size_t i = 0;
    std::size_t scope_end;
    std::size_t end = str_content_.length();
    while (i != end) {
        i = find_loc_start(i);
        if (i == std::string::npos) {
            return;
        }
        scope_end = find_loc_end(i, end);
        this->location_[static_cast<unsigned long>(nb_location - 1)].loc_content_str =
            str_content_.substr(i, scope_end - i);
        i = str_content_.find("location");
        str_content_.erase(i, scope_end + 2 - i);
        i = 0;
        end = str_content_.length();
    }
}

/*
 * Parsing real config
 */

std::string clean_spaces(std::string str)
{
    std::string new_str;
    bool was_space = 0;

    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        if (isspace(*it)) {
            was_space = 1;
        } else {
            if (was_space == 1) {
                new_str.push_back(' ');
                was_space = 0;
            }
            new_str.push_back(*it);
        }
    }
    return new_str;
}

void ServerParser::parse_location_vars()
{
    for (std::vector<LocationContent>::iterator location_it = location_.begin();
         location_it != location_.end(); ++location_it) {
        Config::Location new_location;

        // Adding path to location_path of location
        new_location.location_path = location_it->path;
        new_location = config;
        for (std::vector<std::string>::iterator it = location_it->loc_content.begin();
             it != location_it->loc_content.end(); ++it) {
            std::string tmp_str = clean_spaces(*it);
            std::vector<std::string> directives;
            std::string::size_type pos = 0;

            directives.push_back(tmp_str.substr(0, tmp_str.find(" ")));
            tmp_str.erase(0, tmp_str.find(" ") + 1);

            while ((pos = tmp_str.find(" ")) != std::string::npos) {
                directives.push_back(tmp_str.substr(0, pos));
                tmp_str.erase(0, pos + 1);
            }
            directives.push_back(tmp_str);

            /*
             * Parsing directives
             */
            if (directives.size() > 1) {
                if (directives[0] == "error_page") {
                    Config::Error_page tmp_page;

                    std::vector<std::string>::size_type i = 1;
                    for (; i < directives.size() - 1; ++i) {
                        size_t x;
                        std::istringstream(directives[i]) >> x;
                        tmp_page.code.push_back(x);
                    }
                    tmp_page.uri = directives[i];
                    new_location.error_page.push_back(tmp_page);
                } else if (directives[0] == "client_max_body_size") {
                    std::string tmp_str;

                    if (directives.size() != 2) {
                        throw BadLocationDirective();
                    }
                    for (std::string::iterator it = directives[1].begin();
                         it != directives[1].end(); ++it) {
                        if (isdigit(*it))
                            tmp_str.push_back(*it);
                        else if ((*it == 'm' || *it == 'M') && it + 1 == directives[1].end()) {
                            unsigned long x;
                            std::istringstream(tmp_str) >> x;
                            new_location.client_max_body_size = x * BYTES_IN_MB;
                        } else {
                            throw BadLocationDirective();
                        }
                    }
                    if (new_location.client_max_body_size == 0)
                        throw BadLocationDirective();
                } else if (directives[0] == "limit_except") {
                    config.limit_except.clear();
                    for (std::string::size_type j = 1; j < directives.size(); ++j) {
                        new_location.limit_except.push_back(directives[j]);
                    }
                } else if (directives[0] == "return") {
                    if (directives.size() == 3) {
                        int x;
                        std::istringstream(directives[1]) >> x;
                        if (x < 0)
                            std::cerr
                                << "Error: new_location.return_redirect code is smaller than 0"
                                << std::endl;
                        new_location.return_redirect.code = x;
                        new_location.return_redirect.url = directives[2];

                    } else {
                        throw BadLocationDirective();
                    }
                } else if (directives[0] == "root") {
                    if (directives.size() != 2) {
                        throw BadLocationDirective();
                    }
                    new_location.root = directives[1];
                } else if (directives[0] == "autoindex") {
                    if (directives.size() != 2) {
                        throw BadLocationDirective();
                    }
                    if (directives[1] == "on")
                        new_location.autoindex = true;
                    else if (directives[1] == "off")
                        new_location.autoindex = false;
                    else {
                        throw BadLocationDirective();
                    }
                } else if (directives[0] == "index") {
                    new_location.index.clear();
                    for (std::vector<std::string>::iterator it = directives.begin() + 1;
                         it != directives.end(); ++it) {
                        new_location.index.push_back(*it);
                    }
                } else if (directives[0] == "cgi_ext") {
                    if (directives.size() != 3) {
                        throw BadLocationDirective();
                    }
                    Config::Cgi_ext tmp_cgi;
                    tmp_cgi.extension = directives[1];
                    tmp_cgi.bin_path = directives[2];
                    new_location.cgi_ext.push_back(tmp_cgi);
                } else {
                    throw BadLocationDirective();
                }
            } else {
                throw BadLocationDirective();
            }
        }
        config.location.push_back(new_location);
    }
}

void ServerParser::init_server_vars()
{
    config.listen.port = -1;
    config.client_max_body_size = 1000;
    config.return_redirect.code = -1;
    config.return_redirect.url = "";
    config.root = "";
    config.autoindex = false;
    config.index.push_back("index.html");
}

void ServerParser::parse_server_vars()
{
    this->init_server_vars();

    /*
     * Splitting directives into words
     */
    for (std::vector<std::string>::iterator it = server_content_.begin();
         it != server_content_.end(); ++it) {
        std::string tmp_str = clean_spaces(*it);
        std::vector<std::string> directives;
        std::string::size_type pos = 0;

        directives.push_back(tmp_str.substr(0, tmp_str.find(" ")));
        tmp_str.erase(0, tmp_str.find(" ") + 1);

        while ((pos = tmp_str.find(" ")) != std::string::npos) {
            directives.push_back(tmp_str.substr(0, pos));
            tmp_str.erase(0, pos + 1);
        }
        directives.push_back(tmp_str);

        /*
         * Parsing directives
         */
        if (directives.size() > 1) {
            if (directives[0] == "listen") {
                if (directives.size() != 2)
                    throw BadDirective();
                if (directives[1].find(":") == std::string::npos)
                    throw BadDirective();
                config.listen.combined = directives[1];
                config.listen.address = directives[1].substr(0, directives[1].find(":"));
                std::string tmp_str = directives[1].substr(directives[1].find(":") + 1);
                int x;
                std::istringstream(tmp_str) >> x;
                config.listen.port = x;
            } else if (directives[0] == "server_name") {
                for (std::vector<std::string>::size_type i = 1; i < directives.size(); ++i) {
                    config.server_name.push_back(directives[i]);
                }
            } else if (directives[0] == "error_page") {
                Config::Error_page tmp_page;

                std::vector<std::string>::size_type i = 1;
                for (; i < directives.size() - 1; ++i) {
                    size_t x;
                    std::istringstream(directives[i]) >> x;
                    tmp_page.code.push_back(x);
                }
                tmp_page.uri = directives[i];
                config.error_page.push_back(tmp_page);
            } else if (directives[0] == "client_max_body_size") {
                std::string tmp_str;

                if (directives.size() != 2)
                    throw BadDirective();
                for (std::string::iterator it = directives[1].begin(); it != directives[1].end();
                     ++it) {
                    if (isdigit(*it))
                        tmp_str.push_back(*it);
                    else if ((*it == 'm' || *it == 'M') && it + 1 == directives[1].end()) {
                        unsigned long x;
                        std::istringstream(tmp_str) >> x;
                        config.client_max_body_size = x * BYTES_IN_MB;
                    } else
                        throw BadDirective();
                }
                if (config.client_max_body_size == 0)
                    throw BadDirective();
            } else if (directives[0] == "limit_except") {
                config.limit_except.clear();
                for (std::string::size_type j = 1; j < directives.size(); ++j) {
                    config.limit_except.push_back(directives[j]);
                }
            } else if (directives[0] == "return") {
                if (directives.size() == 3) {
                    int x;
                    std::cerr << "Shouldn't be Config" << std::endl;
                    std::istringstream(directives[1]) >> x;
                    if (x < 0)
                        std::cerr << "Error: return_redirect code is smaller than 0" << std::endl;
                    config.return_redirect.code = x;
                    config.return_redirect.url = directives[2];
                    // config.return_redirect.is_active = true;
                } else
                    throw BadDirective();
            } else if (directives[0] == "root") {
                if (directives.size() != 2)
                    throw BadDirective();
                config.root = directives[1];
            } else if (directives[0] == "autoindex") {
                if (directives.size() != 2)
                    throw BadDirective();
                if (directives[1] == "on")
                    config.autoindex = true;
                else if (directives[1] == "off")
                    config.autoindex = false;
                else
                    throw BadDirective();
            } else if (directives[0] == "index") {
                config.index.clear();
                for (std::vector<std::string>::iterator it = directives.begin() + 1;
                     it != directives.end(); ++it) {
                    config.index.push_back(*it);
                }
            } else if (directives[0] == "cgi_ext") {
                if (directives.size() != 3)
                    throw BadDirective();

                Config::Cgi_ext tmp_cgi;
                tmp_cgi.extension = directives[1];
                tmp_cgi.bin_path = directives[2];
                config.cgi_ext.push_back(tmp_cgi);
            } else {
                throw BadDirective();
            }
        } else
            throw BadDirective();
    }
}

const char* ServerParser::NoSepException::what() const throw()
{
    return ("Error: No ';' found after value\n");
}

const char* ServerParser::SyntaxException::what() const throw()
{
    return ("Error: Syntax error in config file\n");
}

const char* ServerParser::BadLocationDirective::what() const throw()
{
    return ("Error: Directive error in a location context of the config file\n");
}

const char* ServerParser::BadDirective::what() const throw()
{
    return ("Error: Directive error in the config file\n");
}
