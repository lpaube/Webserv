/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/16 16:20:10 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ExceptionBase.hpp"
#include "Request.hpp"
#include "ServerParser.hpp"

class Script
{
public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const char* msg);
    };

public:
    Script(const Config& config, const Request& request);
    ~Script();
    std::string exec();

private:
    void buildCmd(const std::string& path, const Config& config);
    std::string get_ext(const std::string& path);
    void buildEnv(Method method, const Config& config);
    void printEnv() const;

private:
    std::vector<std::string> v_env;
    char** envp;
    char** cmd;
    std::size_t envp_size;
    const Request& request;
};
