/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/04 16:00:34 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerParser.hpp"
#include "http/Request.hpp"
#include "http/Method.hpp"
#include "ExceptionBase.hpp"

class Script
{
	public:
	class Exception : public ExceptionBase
	{
    public:
        Exception(const char* msg);
    };
	public:
		Script(Config& config,  http::Request& request);
		~Script();
		std::string	exec() const;
	private:
		char	**envp;
		char	**cmd;
		http::Request& request;
	private:
		void build_cmd(std::string path, Config& config);
		std::string get_ext(std::string& path);
};
