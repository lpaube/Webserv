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
  this->config = responseConfigs[0];
}
