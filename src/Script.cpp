/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/02 20:45:19 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <string>
#include "http/RequestLine.hpp"

Script::Script(Config& config, http::Request& request, int fd_in, int fd_out) : fd_in(fd_in), fd_out(fd_out){
	(void)this->envp;
	(void)this->cmd;
	(void)this->fd_in;
	(void)this->fd_out;
	http::RequestLine	requestline = request.requestLine();
	this->cmd = build_cmd(requestline.path(), config);
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

char	**build_cmd(std::string path, Config& config){
	std::size_t ext_size = config.cgi_ext.size();
	std::string path_ext = get_ext(path);
	std::vector<std::string>ext;
	for (std::size_t i = 0; i < ext_size; i++){
		if (path_ext.find(config.cgi_ext[i].extension) == true)
			break ;
	}
	char **ret = NULL;
	return ret;
}
