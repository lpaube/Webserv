/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/02 20:36:29 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ServerParser.hpp"
#include "http/Request.hpp"

class Script
{
	public:
		Script(Config& config,  http::Request& request, int fd_in, int fd_out);
		~Script(){}
	private:
		char	**envp;
		char	**cmd;
		int		fd_in;
		int		fd_out;
};

char	**build_cmd(std::string path, Config& config);
