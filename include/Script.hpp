/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/03 14:24:26 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerParser.hpp"
#include "http/Request.hpp"
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
		void	exec() const;
	private:
		char	**envp;
		char	**cmd;
		int		fd[2];
	private:
		void build_cmd(std::string path, Config& config);
		std::string get_ext(std::string& path);
};
