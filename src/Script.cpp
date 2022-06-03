/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/02 21:18:18 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <string>
#include "http/RequestLine.hpp"
#include <string.h>

Script::Script(Config& config, http::Request& request, int fd_in, int fd_out) : fd_in(fd_in), fd_out(fd_out){
	(void)this->envp;
	(void)this->cmd;
	(void)this->fd_in;
	(void)this->fd_out;
	http::RequestLine	requestline = request.requestLine();
	build_cmd(requestline.path(), config);
}

std::string get_ext(std::string path){
	unsigned long	len = path.length();
	unsigned long	save = 0;
	unsigned long	i = 0;
	std::string		ext;
	while (i < len){
		if (path[i] == '/')
			save = i;
		i++;
	}
	while (save < len){
		if (path[save] == '.')
			break ;
		else
			save++;
	}
	//if (save == len)
		//throw NomDexception
	save++;
	ext = path.substr(save);
	return ext;
}

void Script::build_cmd(std::string path, Config& config){

	std::size_t ext_size = config.cgi_ext.size();
	std::string path_ext = get_ext(path);
	bool	found = false;
	std::size_t i = 0;
	while (i < ext_size){
		if (path_ext.find(config.cgi_ext[i].extension) == true){
			found = true;
			break ;
		}
		i++;
	}
	//if (found == false)
		//throw Exception;
	this->cmd = new char *[3];
	this->cmd[0] = strncpy(this->cmd[0], config.cgi_ext[i].bin_path.c_str(), config.cgi_ext[i].bin_path.length());
	std::cout << this->cmd[0] << std::endl;
	this->cmd[1] = strncpy(this->cmd[1], path.c_str(), path.length());
	std::cout << this->cmd[1] << std::endl;
	this->cmd[2] = NULL;
}
