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
#include "LocationContent.hpp"
#include <iostream>
#include <string>

class Config
{
public:
    struct Listen {
        std::string address;
        std::string port;
    };

    struct Error_page {
        std::vector<int> code;
        std::string uri;
    };

    struct Return_redirect {
        int code;
        std::string url;
    };

    struct Cgi_ext {
        std::string extension;
        std::string bin_path;
    };

public:
    Listen listen;
    std::vector<std::string> server_name;
    std::vector<Error_page> error_page;
    unsigned long client_max_body_size; // Represented in bytes
    std::vector<std::string> limit_except;
    Return_redirect return_redirect;
    std::string root;
    bool autoindex;
    std::vector<std::string> index;
    std::vector<Cgi_ext> cgi_ext;

public:
    void print_config()
    {
        // listen
        std::cout << "===== listen =====" << std::endl;
        std::cout << "address: " << listen.address << std::endl;
        std::cout << "port: " << listen.port << std::endl;
        std::cout << std::endl;

        // server_name
        std::cout << "===== server_name =====" << std::endl;
        for (std::vector<std::string>::iterator it = server_name.begin(); it != server_name.end();
             ++it) {
            std::cout << "server name: " << *it << std::endl;
        }
        std::cout << std::endl;

        // error_page
        std::cout << "===== error_page =====" << std::endl;
        int i = 0;
        for (std::vector<Error_page>::iterator it = error_page.begin(); it != error_page.end();
             ++it, ++i) {
            std::cout << "--- error_page: " << i << " ---" << std::endl;
            for (std::vector<int>::iterator it_code = it->code.begin(); it_code != it->code.end();
                 ++it_code) {
                std::cout << "code: " << *it_code << std::endl;
            }
            std::cout << "uri: " << it->uri << std::endl;
        }
        std::cout << std::endl;

        // client_max_body_size
        std::cout << "===== client_max_body_size =====" << std::endl;
        std::cout << "max_body_size: " << client_max_body_size << std::endl;
        std::cout << std::endl;

        // limit_except
        std::cout << "===== limit_except =====" << std::endl;
        for (std::vector<std::string>::iterator it = limit_except.begin(); it != limit_except.end();
             ++it) {
            std::cout << "allowed method: " << *it << std::endl;
        }
        std::cout << std::endl;

        // return
        std::cout << "===== return =====" << std::endl;
        std::cout << "code: " << return_redirect.code << std::endl;
        std::cout << "redirection url: " << return_redirect.url << std::endl;
        std::cout << std::endl;

        // root
        std::cout << "===== root =====" << std::endl;
        std::cout << "root: " << root << std::endl;
        std::cout << std::endl;

        // autoindex
        std::cout << "===== autoindex =====" << std::endl;
        std::cout << "autoindex: " << autoindex << std::endl;
        std::cout << std::endl;

        // index
        std::cout << "===== index =====" << std::endl;
        for (std::vector<std::string>::iterator it = index.begin(); it != index.end(); ++it) {
            std::cout << "index: " << *it << std::endl;
        }
        std::cout << std::endl;

        // cgi_ext
        std::cout << "===== cgi_ext =====" << std::endl;
        i = 0;
        for (std::vector<Cgi_ext>::iterator it = cgi_ext.begin(); it != cgi_ext.end(); ++it) {
            std::cout << "---cgi_ext: " << i << "---" << std::endl;
            std::cout << "extension: " << it->extension << std::endl;
            std::cout << "binary path: " << it->bin_path << std::endl;
        }
        std::cout << std::endl;
    }
};

class ServerParser
{
public:
    ServerParser(std::string::iterator beg, std::string::iterator end);
    ~ServerParser(){};

    void printContent() const;
    void printLocation() const;
    void generate_fake_config();

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

private:
    void buildContent();
    void buildLocation();
    std::size_t findLocStart(std::size_t i);
    std::size_t findLocEnd(std::size_t i, std::size_t end);

    std::vector<std::string> server_content;
    std::vector<LocationContent> location;
    std::string str_content;

public:
    Config config;
    int nb_location;
};
