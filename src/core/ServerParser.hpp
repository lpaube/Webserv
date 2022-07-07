/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:36:33 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 22:13:16 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "Config.hpp"
#include "ExceptionBase.hpp"
#include "LocationContent.hpp"
#include <iostream>
#include <stdint.h>
#include <string>

class ServerParser
{
public:
    ServerParser(std::string::iterator beg, std::string::iterator end);
    ~ServerParser(){};

    void print_content() const;
    void print_location() const;
    void generate_fake_config();
    void init_server_vars();
    void parse_server_vars();
    void parse_location_vars();

    class NoSepException : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

    class SyntaxException : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

    class BadLocationDirective : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

    class BadDirective : public std::exception
    {
    public:
        virtual const char* what() const throw();
    };

private:
    void build_location();
    std::size_t find_loc_start(std::size_t i);
    std::size_t find_loc_end(std::size_t i, std::size_t end);

    std::vector<std::string> server_content_;
    std::vector<LocationContent> location_;
    std::string str_content_;

public:
    Config config;
    int nb_location;
};
