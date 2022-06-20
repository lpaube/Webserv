/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <louis-philippe.aube@hotmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:57:07 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/18 22:10:20 by laube            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "StatusCodes.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <stdio.h>

Response::Response(const Request& request, std::vector<Config>& response_configs)
{
  if (response_configs.size() == 0) {
    std::cerr << "NO CONFIG MATCH" << std::endl;
    throw "No config matches the request";
  }
  this->req = request;
  this->requested_path = req.path();
  this->location_path = "";
  this->status_code = 200;
  this->server = "Anginex/1.0";
  this->content_type = "text/html";
  this->method = req.method();

  init_response(response_configs[0]);
  this->root = config.root;
  full_path = this->root + req.path();
}

void Response::init_response(Config og_config)
{
  config = getSingularConfig(og_config);
  this->root = config.root;
  full_path = this->root + requested_path;
}

bool Response::method_allowed(Method method)
{
  if (config.limit_except.empty())
    return true;
  for (std::vector<std::string>::iterator it = config.limit_except.begin();
      it != config.limit_except.end();
      ++it)
  {
    if ((method == GET && *it == "GET") ||
        (method == POST && *it == "POST") ||
        (method == DELETE && *it == "DELETE") ||
        (method == OPTIONS && *it == "OPETIONS"))
      return true;
  }
  return false;
}

void Response::remove_file()
{
  if (std::remove(full_path.c_str()) != 0)
  {
    std::cout << "Could not delete the file: " << full_path << std::endl;
  }
  else
  {
    std::cout << "Deleted file: " << full_path << std::endl;
  }
}

Config::Location Response::getSingularLocation(std::vector<Config::Location> locations, bool& has_location)
{
  /*
   * Counts the number of directory matches in 
   * the req.path compared to location path
   * And returns the highest.
   */

  Config::Location single_location;
  int best_count = 0;
  int best_slash_count = 0;
  int slash_count;
  int curr_count;
  
  for (std::vector<Config::Location>::iterator it = locations.begin();
      it != locations.end();
      ++it)
  {
    curr_count = 0;
    slash_count = 0;
    
    for (size_t i = 0;
        i < it->location_path.size() /* && requested_path[i] == it->location_path[i] */;
        ++i)
    {
      if (requested_path[i] != it->location_path[i])
      {
        curr_count = 0;
        break;
      }
      if (requested_path[i] == '/')
        slash_count++;
      curr_count++;
    }
    if (curr_count > best_count && slash_count > 0)
    {
      single_location = *it;
      has_location = 1;
      best_count = curr_count;
      best_slash_count = slash_count;
    }
  }
  if (best_slash_count > 1)
    single_location.location_path += "/";
  return single_location;
}

Config Response::getSingularConfig(Config og_config)
{
  bool has_location = false;
  Config::Location single_location;


  single_location = getSingularLocation(og_config.location, has_location);
  if (has_location)
  {
    location_path = single_location.location_path;

    og_config.error_page = single_location.error_page;
    og_config.client_max_body_size = single_location.client_max_body_size;
    og_config.limit_except = single_location.limit_except;
    og_config.root = single_location.root;
    og_config.autoindex = single_location.autoindex;
    og_config.index = single_location.index;
    og_config.cgi_ext = single_location.cgi_ext;
    og_config.return_redirect = single_location.return_redirect;
  }
  return og_config;
}

void Response::setStatusCode(int code)
{
  this->status_code = code;
  this->status_code_msg = StatusCode::get_code_msg(code);
}

void Response::checkErrorCode()
{
  if (status_code < 200 || status_code >= 400)
  {
    for (std::vector<Config::Error_page>::iterator it = config.error_page.begin();
        it != config.error_page.end();
        ++it)
    {
      for (std::vector<int>::iterator it_code = it->code.begin();
          it_code != it->code.end();
          ++it_code)
      {
        if (status_code == *it_code)
        {
          //if (access((it->uri).c_str(), F_OK | R_OK) == 0)
          //{
            //std::cout << "checkErrorCode: Error code file access good!" << std::endl;
            requested_path = it->uri;
            init_response(config);
            //full_path = root + it->uri;
            setHtmlBody();
            return;
          //}
          //std::cout << "checkErrorCode: Error code file access bad!" << std::endl;
        }
      }
    }
    if (status_code >= 300 && status_code < 600)
    {
      std::stringstream body_stream;

      body.clear();
      body_stream << "<h1>Error " << status_code << "</h1>\n\r\n\r";
    }
  }
}

int Response::generate_autoindex(std::ifstream& requested_file, std::stringstream& body_stream)
{
  for (std::vector<std::string>::iterator it = config.index.begin();
      it != config.index.end();
      ++it)
  {
    requested_file.open((full_path + *it).c_str());
    if (requested_file.is_open())
    {
      full_path += *it;
      break;
    }
  }
  /*
   * Making the files hierarchy for autoindex
   */
  if (config.autoindex == true && !requested_file.is_open())
  {
    DIR *dir;

    body_stream << "<h1>Listing files in directory (autoindex=on)</h1>\r\n"
      << "<ul>\r\n";
    struct dirent *ent;
    if ((dir = opendir(full_path.c_str())) != NULL)
    {
      while ((ent = readdir(dir)) != NULL)
      {
        body_stream << "<li>" << ent->d_name << "</li>" << "\r\n";
      }
      closedir(dir);
    }
    body_stream << "</ul>\r\n";
    // Can probably avoid repeat with below
    body_stream << "\r\n";
    body = body_stream.str();
    body_size = body.size();
    if (body_size > getConfig().client_max_body_size)
      setStatusCode(413);
    return 1;
  }
  return 0;
}

void Response::redirect()
{
  status_code = config.return_redirect.code;
}

bool Response::has_return_redirect()
{
  if (config.return_redirect.code != -1)
    return true;
  return false;
}

void Response::setHtmlBody()
{
  std::string line;
  std::stringstream body_stream;
  std::ifstream requested_file;

  body.clear();
  if (has_return_redirect())
  {
    status_code = config.return_redirect.code;
    requested_path = config.return_redirect.url;
    full_path = root + "/" + requested_path;
  }
  /*
   * Check path access
   */
  if (access(full_path.c_str(), F_OK | R_OK) != 0)
  {
    status_code = 404;
    return;
  }

  /*
   * Handle directives index and autoindex
   */
  if (*(full_path.end() - 1) == '/')
  {
    if (generate_autoindex(requested_file, body_stream))
      return;
  }
  else
  {
    std::string path_extension = full_path.substr(full_path.find_last_of(".") + 1);
    if (path_extension == "jpeg" || path_extension == "jpg" ||
        path_extension == "png" || path_extension == "gif")
    {
      file_type = "IMAGE";
      requested_file.open(full_path.c_str(), std::ios_base::in | std::ios_base::binary);
    }
    else
      requested_file.open(full_path.c_str());
  }

  if (!requested_file.is_open()) {
    setStatusCode(400);
  } else {
    // body.clear();
    if (file_type == "IMAGE")
    {
      body_stream << requested_file.rdbuf();
    }
    else
    {
      while (getline(requested_file, line)) {
        body_stream << line << "\r\n";
      }
    }
    requested_file.close();
    body_stream << "\r\n";
    body = body_stream.str();
    body_size = body.size();
    if (body_size > getConfig().client_max_body_size)
      setStatusCode(413);
  }
}

void Response::setContentType()
{
  std::string path_extension = full_path.substr(full_path.find_last_of(".") + 1);
  if (path_extension == "jpg" || path_extension == "jpeg")
    content_type = "image/jpeg";
  else if (path_extension == "png")
    content_type = "image/png";
  else if (path_extension == "gif")
    content_type = "image/gif";
  else if (path_extension == "mp4")
    content_type = "video/mp4";
  else if (path_extension == "css")
    content_type = "text/css";
  else if (path_extension == "js")
    content_type = "text/javascript";
  else if (path_extension == "md")
    content_type = "text/markdown";
}

void Response::setDate()
{
  char buf[1000];

  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%S %Z", &tm);
  date_now = std::string(buf);
}

void Response::setHost()
{
  Request::header_iterator it = req.find_header("host");
  host = it->second;
}

int Response::setAllow()
{
  if (config.limit_except.size() == 0)
    return 0;
  allow = config.limit_except[0];
  for (std::vector<std::string>::iterator it = config.limit_except.begin() + 1;
      it != config.limit_except.end();
      ++it)
  {
    allow += ", ";
    allow += *it;
  }
  return 1;
}

void Response::setHtmlHeader()
{
  std::stringstream header_stream;

  setContentType();
  setDate();
  setHost();
  header_stream << "HTTP/1.1 " << status_code << " " << StatusCode::get_code_msg(status_code)
    << "\r\n"
    << "Access-Control-Allow-Origin: *\r\n";
  header_size = header_stream.str().size();
  header_stream << "Date: " << date_now << "\r\n";
  header_stream << "Host: " << host << "\r\n";
  if (setAllow())
  {
    header_stream << "Allow: " << allow << "\r\n";
  }
  header_stream << "Content-Length: " << body_size << "\r\n";
  header_stream << "Content-Type: " << content_type << "\r\n";
  header_stream << "Accept-Ranges: bytes" << "\r\n";
  header_stream << "Server: " << server << "\r\n";
  header_stream << "\r\n";
  header = header_stream.str();
  full_content = header + body;
}
