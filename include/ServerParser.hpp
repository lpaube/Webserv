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
#include <string>
#include "LocationContent.hpp"
#include <iostream>

class ServerParser{
	public:
		ServerParser(std::string::iterator beg, std::string::iterator end);
		~ServerParser(){};
	private:
		void	buildContent();
		std::vector<std::string> server_content;
		std::vector<LocationContent>	location;
		std::string	str_content;

    /*
     * Parsing of vector server_content
     */
    void parse_config_vars()
    {
      std::vector<std::string>::iterator beg_ite = server_content.begin();
      std::vector<std::string>::iterator end_ite = server_content.end();
      for (; beg_ite != end_ite; ++beg_ite)
      {
        std::string key_word = (beg_ite->substr(0, beg_ite->find(" ")));
        if (key_word == "listen")
          std::cout << "LISTEN!" << std::endl;
        if (key_word == "server_name")
          std::cout << "SERVER_NAME!" << std::endl;
        else
          std::cout << "Nothing!" << std::endl;
      }
    }
};

class Config
{
  private:
    class Listen
    {
      std::string address;
      std::string port;
    };

    class Error_page
    {
      std::vector<int> code;
      std::string uri;
    };

    class Return_redirect
    {
      public:
        int code;
        std::string url;
    };

    class Cgi_ext
    {
      std::string extension;
      std::string bin_path;
    };

  public:
    Listen                    listen;
    std::vector<std::string>  server_name;
    Error_page                error_page;
    unsigned long int         client_max_body_size; // Represented in bytes
    std::vector<std::string>  limit_except;
    Return_redirect           return_redirect;
    std::string               root;
    bool                      autoindex;
    std::vector<std::string>  index;
    std::vector<Cgi_ext>      cgi_ext;
};
