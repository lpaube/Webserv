/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:56:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/08 15:57:04 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <string>
#include <map>
#include "Config.hpp"
#include "sock/Connection.hpp"
#include <sstream>

#pragma once

class Response
{
  public:
    Response(/* args */) {}
    Response(sock::Connection c, std::vector<Config>& configs);
    ~Response() {}
    std::string getHeader() const;
    Config getConfig() const {return config;}
    void	setContentLength(std::size_t length);

  private:
    void  createCodeMsg();
    void  buildHeaderString();

  public:
    std::stringstream body;
    std::stringstream header;

  private:
    //std::map<int, std::string> codeList;
    Config      config;
    size_t body_size;
    size_t header_size;
    std::size_t	code;
    //std::string location;
    //std::string headerString;
    //std::string codeMsg;
};
