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

Response::Response(const Request& request, const std::vector<Config>& response_configs)
  : req_(request)
  , status_code_(200)
  , method_(req_.method())
  , requested_path_(req_.path())
  , content_type_("text/html")
  , server_("Anginex/1.0")
  , location_path_("")
{
  if (response_configs.size() == 0) {
    throw "No config matches the request";
  }
  config_ = response_configs[0];
  generate_singular_config();
}

Config Response::get_config() const
{
  return config_;
}

int Response::get_status_code() const
{
  return status_code_;
}

std::string Response::get_status_code_msg() const
{
  return status_code_msg_;
}

std::string Response::get_location_path() const
{
  return location_path_;
}

std::string Response::get_full_path() const
{
  return full_path_;
}

Method Response::get_method() const
{
  return method_;
}

bool Response::method_allowed(Method method) const
{
  if (config_.limit_except.empty())
    return true;
  for (std::vector<std::string>::const_iterator it = config_.limit_except.begin();
      it != config_.limit_except.end();
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
  if (std::remove(full_path_.c_str()) != 0)
  {
    status_code_ = 204;
  }
  else
  {
    status_code_ = 200;
    body = "<h1>The file has been deleted: " + full_path_ + "</h1>\r\n";
    body_size_ = body.size();
    full_path_ = "delete_message.html";
  }
}

Config::Location Response::generate_singular_location(const std::vector<Config::Location>& locations, bool& has_location)
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
  
  for (std::vector<Config::Location>::const_iterator it = locations.begin();
      it != locations.end();
      ++it)
  {
    curr_count = 0;
    slash_count = 0;
    
    for (size_t i = 0;
        i < it->location_path.size() /* && requested_path[i] == it->location_path[i] */;
        ++i)
    {
      if (requested_path_[i] != it->location_path[i])
      {
        curr_count = 0;
        break;
      }
      if (requested_path_[i] == '/')
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

void Response::generate_singular_config()
{
  bool has_location = false;
  Config::Location single_location;


  single_location = generate_singular_location(config_.location, has_location);
  if (has_location)
  {
    location_path_ = single_location.location_path;
    config_ = single_location;
  }
  root_ = config_.root;
  full_path_ = root_ + requested_path_;
}

void Response::set_status_code(int code)
{
  this->status_code_ = code;
  this->status_code_msg_ = StatusCode::get_code_msg(code);
}

void Response::check_error_code()
{
  if (status_code_ >= 300 && status_code_ < 600)
  {
    for (std::vector<Config::Error_page>::iterator it = config_.error_page.begin();
        it != config_.error_page.end();
        ++it)
    {
      for (std::vector<int>::iterator it_code = it->code.begin();
          it_code != it->code.end();
          ++it_code)
      {
        if (status_code_ == *it_code)
        {
          requested_path_ = it->uri;
          generate_singular_config();
          set_html_body();
          return;
        }
      }
    }
  }
}

int Response::generate_autoindex(std::ifstream& requested_file, std::stringstream& body_stream)
{
  for (std::vector<std::string>::iterator it = config_.index.begin();
      it != config_.index.end();
      ++it)
  {
    requested_file.open((full_path_ + *it).c_str());
    if (requested_file.is_open())
    {
      full_path_ += *it;
      break;
    }
  }
  /*
   * Making the files hierarchy for autoindex
   */
  if (config_.autoindex == true && !requested_file.is_open())
  {
    DIR *dir;

    body_stream << "<h1>Listing files in directory (autoindex=on)</h1>\r\n"
      << "<ul>\r\n";
    struct dirent *ent;
    if ((dir = opendir(full_path_.c_str())) != NULL)
    {
      while ((ent = readdir(dir)) != NULL)
      {
        body_stream << "<li><a href=\"" << requested_path_ + ent->d_name << "\">" << ent->d_name << "</a></li>" << "\r\n";
      }
      closedir(dir);
    }
    body_stream << "</ul>\r\n";
    body_stream << "\r\n";
    body = body_stream.str();
    body_size_ = body.size();
    if (body_size_ > get_config().client_max_body_size)
      set_status_code(413);
    return 1;
  }
  return 0;
}

void Response::redirect()
{
  status_code_ = config_.return_redirect.code;
}

bool Response::has_return_redirect() const
{
  if (config_.return_redirect.code != -1)
    return true;
  return false;
}

void Response::set_html_body()
{
  std::string line;
  std::stringstream body_stream;
  std::ifstream requested_file;

  body.clear();
  if (has_return_redirect())
  {
    status_code_ = config_.return_redirect.code;
    requested_path_ = config_.return_redirect.url;
    full_path_ = root_ + "/" + requested_path_;
  }
  /*
   * Check path access
   */
  if (access(full_path_.c_str(), F_OK | R_OK) != 0)
  {
    status_code_ = 404;
    return;
  }

  /*
   * Handle directives index and autoindex
   */
  if (*(full_path_.end() - 1) == '/')
  {
    if (generate_autoindex(requested_file, body_stream))
      return;
  }
  else
  {
    size_t ext_pos = full_path_.find_last_of(".");
    if (ext_pos == std::string::npos)
    {
      throw("Big error in finding extension position");
    }
    std::string path_extension = full_path_.substr(ext_pos + 1);
    if (path_extension == "jpeg" || path_extension == "jpg" ||
        path_extension == "png" || path_extension == "gif")
    {
      file_type_ = "IMAGE";
      requested_file.open(full_path_.c_str(), std::ios_base::in | std::ios_base::binary);
    }
    else
      requested_file.open(full_path_.c_str());
  }

  if (!requested_file.is_open()) {
    set_status_code(400);
  } else {
    // body.clear();
    if (file_type_ == "IMAGE")
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
    body_size_ = body.size();
    if (body_size_ > get_config().client_max_body_size)
      set_status_code(413);
  }
}

void Response::set_content_type()
{
  size_t ext_pos = full_path_.find_last_of(".");
  if (ext_pos == std::string::npos)
  {
    throw("Big error in finding extension position");
  }
  std::string path_extension = full_path_.substr(ext_pos + 1);
  if (path_extension == "jpg" || path_extension == "jpeg")
    content_type_ = "image/jpeg";
  else if (path_extension == "png")
    content_type_ = "image/png";
  else if (path_extension == "gif")
    content_type_ = "image/gif";
  else if (path_extension == "mp4")
    content_type_ = "video/mp4";
  else if (path_extension == "css")
    content_type_ = "text/css";
  else if (path_extension == "js")
    content_type_ = "text/javascript";
  else if (path_extension == "md")
    content_type_ = "text/markdown";
}

void Response::set_date()
{
  char buf[1000];

  time_t now = time(0);
  struct tm tm = *gmtime(&now);
  strftime(buf, sizeof buf, "%a, %d %b %Y %H:%S %Z", &tm);
  date_now_ = std::string(buf);
}

void Response::set_host()
{
  Request::header_iterator it = req_.find_header("host");
  if (it != req_.headers_end())
    host_ = it->second;
  else
    host_ = "";
}

int Response::set_allow()
{
  if (config_.limit_except.size() == 0)
    return 0;
  allow_ = config_.limit_except[0];
  for (std::vector<std::string>::iterator it = config_.limit_except.begin() + 1;
      it != config_.limit_except.end();
      ++it)
  {
    allow_ += ", ";
    allow_ += *it;
  }
  return 1;
}

void Response::set_html_header()
{
  std::stringstream header_stream;

  set_content_type();
  set_date();
  set_host();
  header_stream << "HTTP/1.1 " << status_code_ << " " << StatusCode::get_code_msg(status_code_)
    << "\r\n"
    << "Access-Control-Allow-Origin: *\r\n";
  header_size_ = header_stream.str().size();
  header_stream << "Date: " << date_now_ << "\r\n";
  header_stream << "Host: " << host_ << "\r\n";
  if (set_allow())
  {
    header_stream << "Allow: " << allow_ << "\r\n";
  }
  header_stream << "Content-Length: " << body_size_ << "\r\n";
  header_stream << "Content-Type: " << content_type_ << "\r\n";
  header_stream << "Accept-Ranges: bytes" << "\r\n";
  header_stream << "Server: " << server_ << "\r\n";
  header_stream << "\r\n";
  header = header_stream.str();
  full_content = header + body;
}

