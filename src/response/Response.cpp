/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:57:07 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/08 15:57:16 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response(sock::Connection c, std::vector<Config>& configs)
{
	std::vector<Config> responseConfigs;
	http::HeaderMap headers = c.request().headers();
	http::HeaderMap::const_iterator it = headers.get("host");
  std::string host = it->second;
  for(unsigned long i = 0; i < configs.size(); i++){
    if (host == configs[i].listen.combined){
      responseConfigs.push_back(configs[i]);
		}
	}
  if (responseConfigs.size() == 0)
    throw "No config matches the request";
  full_path = "." + c.request().requestLine().path();
  this->config = responseConfigs[0];
}

size_t Response::getBodySize() const {
  return body.str().size();
}

void Response::setStatusCode(size_t code)
{
  this->status_code = code;
}

void Response::setHtmlBody()
{
  std::string line;
  std::ifstream requested_file(full_path.c_str());

  if (!requested_file.is_open())
  {
    setStatusCode(400);
    std::cerr << "There was an error when trying to open the html file." << std::endl;
  }
  else
  {
    setStatusCode(200);
    //body.clear();
    while (getline(requested_file, line))
    {
      body << line << std::endl;
    }
    requested_file.close();
    if (getBodySize() > getConfig().client_max_body_size)
      setStatusCode(413);
  }
}

void Response::setHtmlHeader()
{
  
}
