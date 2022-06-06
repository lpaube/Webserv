/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:47:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/02 13:59:01 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include "ConfigParser.hpp"
#include "LocationContent.hpp"
#include "ServerParser.hpp"
#include "Utils.hpp"

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end) : str_content(beg, end){
  nb_location = 0;
  buildLocation();
  for (unsigned long i = 0; i < static_cast<unsigned long>(this->nb_location); i++){
    std::string line = this->location[i].loc_content_str;
    vectorize_content(this->location[i].loc_content, line);
  }
  if (vectorize_content(this->server_content, str_content) == false)
    throw NoSepException();
  //DEBUGING SERVER/LOCATION PARSER
  //printContent();
  //printLocation();
}

void	ServerParser::printLocation() const{
  unsigned long size = location.size();
  std::cout << "PRINTING LOCATION CONTENT\n"; 
  for (unsigned long i = 0; i < size; i++){
    std::cout << "LOCATION : " << location[i].path << '\n';
    for(unsigned long j = 0; j < location[i].loc_content.size(); j++){
      std::cout << location[i].loc_content[j] << '\n';
    }
  }
}
void	ServerParser::printContent() const{
  int size = server_content.size();
  std::cout << "PRINTING SERVER CONTENT\n"; 
  for(int i = 0; i < size; i++){
    std::cout << server_content[static_cast<unsigned long>(i)] << std::endl;
  }
}

std::size_t	ServerParser::findLocStart(std::size_t i){
  i = str_content.find("location");
  if (i == std::string::npos)
    return i;
  nb_location++;
  location.resize(static_cast<unsigned long>(nb_location));
  i += 8;
  while (str_content[i] > 0 && str_content[i] < 33)
    i++;
  while (str_content[i] && str_content[i] != '{' && str_content[i] >= 33){
    location[static_cast<unsigned long>(nb_location - 1)].path.push_back(str_content[i]);
    i++;
  }
  while (str_content[i] > 0 && str_content[i] < 33)
    i++;
  if (str_content[i] != '{')
    throw SyntaxException();
  i++;
  return i;
}

std::size_t ServerParser::findLocEnd(std::size_t i, std::size_t end){
  while (i != end){
    if (this->str_content[i] == '{')
      throw SyntaxException();
    if (this->str_content[i] == '}')
      return i - 1;
    i++;
  }
  throw SyntaxException();
}

void	ServerParser::buildLocation(){
  std::size_t i = 0;
  std::size_t	scope_end;
  std::size_t end = str_content.length();
  while (i != end){
    i = findLocStart(i);
    if (i == std::string::npos){
      return ;
    }
    scope_end = findLocEnd(i, end);
    this->location[static_cast<unsigned long>(nb_location - 1)].loc_content_str = str_content.substr(i, scope_end - i);
    i = str_content.find("location");
    str_content.erase(i , scope_end + 2 - i);
    i = 0;
    end = str_content.length();
  }
}

void	ServerParser::buildContent(){
  std::string::iterator start = str_content.begin();
  std::string::iterator end = str_content.end();
  std::string::iterator current = start;

  while (start != end){
    while (*start > 0 && *start < 33)
      start++;
    current = start;
    if (start != end){
      while(*current != ';'){
        if (current == end)
          throw NoSepException();
        current++;
      }
      std::string add(start, current);
      server_content.push_back(trim_white_spaces(add));
      start = current + 1;
    }
  }
}

/*
 * Generating fake config
 */
void ServerParser::generate_fake_config()
{
  // listen
  config.listen.address = "127.0.0.1";
  config.listen.port = 8000;

  // server_name
  config.server_name.push_back("example.com");
  config.server_name.push_back("www.example.com");

  // error_page
  Config::Error_page error_page1;
  error_page1.code.push_back(404);
  error_page1.code.push_back(442);
  error_page1.uri = "/40x.html";

  Config::Error_page error_page2;
  error_page2.code.push_back(505);
  error_page2.code.push_back(506);
  error_page2.uri = "/50x.html";

  config.error_page.push_back(error_page1);
  config.error_page.push_back(error_page2);

  // client_max_body_size
  config.client_max_body_size = 8000000;

  // limit_except
  config.limit_except.push_back("GET");
  config.limit_except.push_back("POST");

  // return
  config.return_redirect.code = 404;
  config.return_redirect.url = "/error404";

  // root
  config.root = "/data/w5";

  // autoindex
  config.autoindex = true;

  // index
  config.index.push_back("index");
  config.index.push_back("index.html");

  // cgi_ext
  Config::Cgi_ext cgi_ext1;
  cgi_ext1.extension = ".php";
  cgi_ext1.bin_path = "/usr/bin/php";

  Config::Cgi_ext cgi_ext2;
  cgi_ext2.extension = ".py";
  cgi_ext2.bin_path = "/usr/bin/python";

  config.cgi_ext.push_back(cgi_ext1);
  config.cgi_ext.push_back(cgi_ext2);
}

/*
 * Parsing real config
 */

void ServerParser::init_config()
{
  config.listen.port = -1;
  config.server_name.push_back("");
  config.client_max_body_size = 1000;
  config.return_redirect.code = -1;
  config.root = "html";
  config.autoindex = false;
  config.index.push_back("index.html");
}

std::string clean_spaces(std::string str)
{
  std::string new_str;
  bool was_space = 0;

  for (std::string::iterator it = str.begin();
      it != str.end();
      ++it)
  {
    if (isspace(*it))
    {
      was_space = 1;
    }
    else
    {
      if (was_space == 1)
      {
        new_str.push_back(' ');
        was_space = 0;
      }
      new_str.push_back(*it);
    }
  }
  return new_str;
}

void ServerParser::parse_config_vars()
{
  this->init_config();

  for (std::vector<std::string>::iterator it = server_content.begin();
      it != server_content.end();
      ++it)
  {
    std::string tmp_str = clean_spaces(*it);
    std::vector<std::string> directives;
    std::string::size_type pos = 0;

    directives.push_back(tmp_str.substr(0, tmp_str.find(" ")));
    tmp_str.erase(0, tmp_str.find(" ") + 1);

    while ((pos = tmp_str.find(" ")) != std::string::npos)
    {
      directives.push_back(tmp_str.substr(0, pos));
      tmp_str.erase(0, pos + 1);
    }
    directives.push_back(tmp_str);


    if (directives.size() > 0)
    {
      if (directives[0] == "listen")
      {
        std::cout << "LISTEN!" << std::endl;
      }
      else if (directives[0] == "server_name")
      {
        std::cout << "There is a server_name" << std::endl;
      }
      else if (directives[0] == "error_page")
      {
        std::cout << "There is a error_page" << std::endl;
      }
      else if (directives[0] == "client_max_body_size")
      {
        std::cout << "There is a client_max_body_size" << std::endl;
      }
      else if (directives[0] == "limit_except")
      {
        std::cout << "limit_except" << std::endl;
      }
      else if (directives[0] == "return")
      {
        std::cout << "return" << std::endl;
      }
      else if (directives[0] == "root")
      {
        std::cout << "root" << std::endl;
      }
      else if (directives[0] == "autoindex")
      {
        std::cout << "autoindex" << std::endl;
      }
      else if (directives[0] == "index")
      {
        std::cout << "index" << std::endl;
      }
      else if (directives[0] == "cgi_ext")
      {
        std::cout << "cgi_ext" << std::endl;
      }
    }
  }
}

const char* ServerParser::NoSepException::what() const throw(){
  return ("Error: No ';' found after value\n");
}

const char* ServerParser::SyntaxException::what() const throw(){
  return ("Error: Syntax error in config file\n");
}
