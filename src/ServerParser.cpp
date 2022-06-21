/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <louis-philippe.aube@hotmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/01 10:47:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/18 22:04:42 by laube            ###   ########.fr       */
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

#define BYTES_IN_MB 1000000

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end)
  : str_content_(beg, end)
{
  nb_location = 0;
  build_location();
  for (unsigned long i = 0; i < static_cast<unsigned long>(this->nb_location); i++) {
    std::string line = this->location_[i].loc_content_str;
    vectorize_content(this->location_[i].loc_content, line);
  }
  if (vectorize_content(this->server_content_, str_content_) == false)
    throw NoSepException();
  // Building config
  try {
    parse_server_vars();
    parse_location_vars();
  } catch (const char* s) {
    std::cout << "ERROR: " << s << std::endl;
  }
}

void ServerParser::print_location() const
{
  unsigned long size = location_.size();
  std::cout << "PRINTING LOCATION CONTENT\n";
  for (unsigned long i = 0; i < size; i++) {
    std::cout << "LOCATION : " << location_[i].path << '\n';
    for (unsigned long j = 0; j < location_[i].loc_content.size(); j++) {
      std::cout << location_[i].loc_content[j] << '\n';
    }
  }
}

void ServerParser::print_content() const
{
  int size = server_content_.size();
  std::cout << "PRINTING SERVER CONTENT\n";
  for (int i = 0; i < size; i++) {
    std::cout << server_content_[static_cast<unsigned long>(i)] << std::endl;
  }
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
    
    //Adding path to location_path of location
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
            throw("client_max_body_size: wrong args number");
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
              throw("client_max_body_size not a valid input");
            }
          }
        } else if (directives[0] == "limit_except") {
          for (std::string::size_type j = 1; j < directives.size(); ++j) {
            new_location.limit_except.push_back(directives[j]);
          }
        } else if (directives[0] == "return") {
          /*
          if (directives.size() == 2)
            new_location.return_redirect.url = directives[1];
            */
          if (directives.size() == 3) {
            int x;
            std::istringstream(directives[1]) >> x;
            if (x < 0)
              std::cerr << "Error: new_location.return_redirect code is smaller than 0" << std::endl;
            new_location.return_redirect.code = x;
            new_location.return_redirect.url = directives[2];
            //new_location.return_redirect.is_active = true;

          } else {
            std::cerr << "LP ERROR 3" << std::endl;
            throw("return_redirect: error..");
          }
        } else if (directives[0] == "root") {
          if (directives.size() != 2) {
            throw("root: wrong number of args");
          }
          new_location.root = directives[1];
        } else if (directives[0] == "autoindex") {
          if (directives.size() != 2) {
            throw("autoindex: wrong number of args");
          }
          if (directives[1] == "on")
            new_location.autoindex = true;
          else if (directives[1] == "off")
            new_location.autoindex = false;
          else {
            throw("autoindex: invalid argument");
          }
        } else if (directives[0] == "index") {
          new_location.index.clear();
          for (std::vector<std::string>::iterator it = directives.begin() + 1;
              it != directives.end(); ++it) {
            new_location.index.push_back(*it);
          }
        } else if (directives[0] == "cgi_ext") {
          if (directives.size() != 3) {
            throw("cgi_ext: wrong number of args");
          }
          Config::Cgi_ext tmp_cgi;
          tmp_cgi.extension = directives[1];
          tmp_cgi.bin_path = directives[2];
          new_location.cgi_ext.push_back(tmp_cgi);
        } else {
          throw("new_location file: there is an invalid directive");
        }
      } else {
        throw("In new_location file: Not enough arguments provided");
      }
    }
      config.location.push_back(new_location);
  }
}

  void ServerParser::init_server_vars()
  {
    config.listen.port = -1;
    // config.server_name.push_back("");
    config.client_max_body_size = 1000;
    //config.return_redirect.is_active = false;
    config.return_redirect.code = -1;
    config.return_redirect.url = "";
    config.root = "";
    config.autoindex = false;
    config.index.push_back("index.html");
    //config.limit_except.push_back("GET");
  }

void ServerParser::parse_server_vars()
{
  this->init_server_vars();

  /*
   * Splitting directives into words
   */
  for (std::vector<std::string>::iterator it = server_content_.begin(); it != server_content_.end();
      ++it) {
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
            throw("Listen: wrong arg number");
          if (directives[1].find(":") == std::string::npos)
            throw("Listen: no port number");
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
            throw("client_max_body_size: wrong args number");
          for (std::string::iterator it = directives[1].begin(); it != directives[1].end();
              ++it) {
            if (isdigit(*it))
              tmp_str.push_back(*it);
            else if ((*it == 'm' || *it == 'M') && it + 1 == directives[1].end()) {
              unsigned long x;
              std::istringstream(tmp_str) >> x;
              config.client_max_body_size = x * BYTES_IN_MB;
            } else
              throw("client_max_body_size not a valid input");
          }
        } else if (directives[0] == "limit_except") {
          //throw("Should not be limit_except in server context");
          for (std::string::size_type j = 1; j < directives.size(); ++j) {
            config.limit_except.push_back(directives[j]);
          }
        } else if (directives[0] == "return") {
          /*
          if (directives.size() == 2)
            config.return_redirect.url = directives[1];
            */
          if (directives.size() == 3) {
            int x;
            std::cerr << "Shouldn't be Config" << std::endl;
            std::istringstream(directives[1]) >> x;
            if (x < 0)
              std::cerr << "Error: return_redirect code is smaller than 0" << std::endl;
            config.return_redirect.code = x;
            config.return_redirect.url = directives[2];
            //config.return_redirect.is_active = true;
          } else
            throw("return_redirect: error..");
        } else if (directives[0] == "root") {
          if (directives.size() != 2)
            throw("root: wrong number of args");
          config.root = directives[1];
        } else if (directives[0] == "autoindex") {
          if (directives.size() != 2)
            throw("autoindex: wrong number of args");
          if (directives[1] == "on")
            config.autoindex = true;
          else if (directives[1] == "off")
            config.autoindex = false;
          else
            throw("autoindex: invalid argument");
        } else if (directives[0] == "index") {
          config.index.clear();
          for (std::vector<std::string>::iterator it = directives.begin() + 1;
              it != directives.end(); ++it) {
            config.index.push_back(*it);
          }
        } else if (directives[0] == "cgi_ext") {
          if (directives.size() != 3)
            throw("cgi_ext: wrong number of args");

          Config::Cgi_ext tmp_cgi;
          tmp_cgi.extension = directives[1];
          tmp_cgi.bin_path = directives[2];
          config.cgi_ext.push_back(tmp_cgi);
        } else {
          throw("config file: there is an invalid directive");
        }
      } else
        throw("In config file: Not enough arguments provided");
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
