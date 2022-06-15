/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:57:07 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/15 12:57:17 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "StatusCodes.hpp"

Response::Response(http::Request& request, std::vector<Config>& configs)
{
  /*
	std::vector<Config> responseConfigs;
	http::HeaderMap headers = request.headers();
	http::HeaderMap::const_iterator it = headers.get("host");
  std::string host = it->second;
  for(unsigned long i = 0; i < configs.size(); i++){
    if (host == configs[i].listen.combined){
      responseConfigs.push_back(configs[i]);
		}
	}
  if (responseConfigs.size() == 0)
  {
    std::cerr << "NO CONFIG MATCH" << std::endl;
    throw "No config matches the request";
  }
  full_path = "." + request.requestLine().path();
  */
  (void)request;
  this->config = configs[0];
}

void Response::setStatusCode(size_t code)
{
  this->status_code = code;
  this->status_code_msg = StatusCodes::getCodeMsg(code);
}

void Response::setHtmlBody()
{
  std::string line;
  std::stringstream body_stream;
  std::ifstream requested_file(full_path.c_str());

  if (!requested_file.is_open())
  {
    std::cerr << full_path.c_str() << std::endl;
    setStatusCode(400);
    std::cerr << "There was an error when trying to open the html file." << std::endl;
  }
  else
  {
    setStatusCode(200);
    //body.clear();
    while (getline(requested_file, line))
    {
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

void Response::setHtmlHeader()
{
  std::stringstream header_stream;

  header_stream << "HTTP/1.1 " << status_code << " " << StatusCodes::getCodeMsg(status_code)
    << "\r\n" << "Access-Control-Allow-Origin: *\r\n";
  header_size = header_stream.str().size();
  header_stream << "Content Length: " << header_size + body_size << "\r\n\r\n";
  header = header_stream.str();
  full_content = header + body;
}
