/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:57:07 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/17 16:36:41 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "StatusCodes.hpp"
#include <fstream>
#include <sstream>
#include <dirent.h>

Response::Response(const Request& request, std::vector<Config>& response_configs)
{
    full_path = "." + request.path();
    this->config = response_configs[0];
    this->status_code = 200;
    this->content_type = "text/html";
}

void Response::setStatusCode(size_t code)
{
    this->status_code = code;
    this->status_code_msg = StatusCode::get_code_msg(code);
}

void Response::checkErrorCode()
{
  std::cout << "@@@This is the status_code: " << status_code << std::endl;
  if (status_code != 200)
  {
    for (std::vector<Config::Error_page>::iterator it = config.error_page.begin();
        it != config.error_page.end();
        ++it)
    {
      for (std::vector<size_t>::iterator it_code = it->code.begin();
          it_code != it->code.end();
          ++it_code)
      {
        if (status_code == *it_code)
        {
          std::cout << "Status code match!" << std::endl;
          if (access((it->uri).c_str(), F_OK | R_OK) == 0)
          {
            std::cout << "Erro code file access good!" << std::endl;
            full_path = it->uri;
            setHtmlBody();
            return;
          }
          std::cout << "Erro code file access bad!" << std::endl;
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

void Response::setHtmlBody()
{
    std::string line;
    std::stringstream body_stream;
    std::ifstream requested_file;

    body.clear();
    /*
     * Check path access
     */
    if (access(full_path.c_str(), F_OK | R_OK) != 0)
    {
      status_code = 404;
      std::cout << "This is a 404! (Response::setHtmlBody)" << std::endl;
      return;
    }

    /*
     * Handle directives index and autoindex
     */
    if (*(full_path.end() - 1) == '/')
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
        return;
      }
    }
    else
    {
      requested_file.open(full_path.c_str());
    }

    if (!requested_file.is_open()) {
      setStatusCode(400);
      std::cerr << "There was an error when trying to open the html file." << std::endl;
    } else {
      setStatusCode(200);
      // body.clear();
      while (getline(requested_file, line)) {
        body_stream << line << "\r\n";
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
  std::string extension = full_path.substr(full_path.find_last_of(".") + 1);
  std::cout << "@@@@@This is extension: " << extension << std::endl;
  if (extension == "jpg" || extension == "jpeg")
    content_type = "image/jpeg";
  else if (extension == "png")
    content_type = "image/png";
  else if (extension == "gif")
    content_type = "image/gif";
  else if (extension == "mp4")
    content_type = "video/mp4";
  else if (extension == "css")
    content_type = "text/css";
  else if (extension == "js")
    content_type = "text/javascript";
  else if (extension == "md")
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

void Response::setHtmlHeader()
{
  std::stringstream header_stream;

  setContentType();
  setDate();
  header_stream << "HTTP/1.1 " << status_code << " " << StatusCode::get_code_msg(status_code)
    << "\r\n"
    << "Access-Control-Allow-Origin: *\r\n";
  header_size = header_stream.str().size();
  header_stream << "Date: " << date_now << "\r\n";
  header_stream << "Content Length: " << body_size << "\r\n";
  header_stream << "Content Type: " << content_type << "\r\n";
  header_stream << "\r\n";
  header = header_stream.str();
  full_content = header + body;
}
