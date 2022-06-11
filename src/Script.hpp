/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/11 16:00:46 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ExceptionBase.hpp"
#include "ServerParser.hpp"
#include "http/Method.hpp"
#include "http/Request.hpp"

class Script
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Script(Config& config, http::Request& request);
    ~Script();
    std::string exec();

private:
    char** envp;
    char** cmd;
    http::Request& request;

private:
    void buildCmd(std::string path, Config& config);
    std::string get_ext(std::string& path);
	void buildEnv(http::Method& method);
};
