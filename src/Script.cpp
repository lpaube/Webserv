/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/03 14:56:19 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <string>
#include "http/RequestLine.hpp"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

Script::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

Script::Script(Config& config, http::Request& request) {
	(void)this->envp;
	this->cmd = new char *[3];
	http::RequestLine	requestline = request.requestLine();
	build_cmd(requestline.path(), config);
}

Script::~Script(){
	delete[] cmd;
}

void	Script::exec() const{
	pid_t	id;
	int		status;
	int		save[2];
	save[0] = dup(STDIN_FILENO);
	save[1] = dup(STDOUT_FILENO);
	dup2(fd[1], STDOUT_FILENO);
	dup2(fd[0], STDIN_FILENO);
	pipe((int *)fd);
	id = fork();
	if (id < 0)
		throw Exception("Error fatal, fork");
	if (id == 0){
		execve(cmd[0], cmd, envp);
		throw Exception("Error fatal, execve");
	}
	else
		waitpid(id, &status, 0);
	//READ
}

std::string Script::get_ext(std::string& path){
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
	if (save == len)
		throw Exception("Error: No script extention found");
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
	std::string msg = "Error: script extension ." + path_ext;
	if (found == false)
		throw Exception(msg.c_str());
	this->cmd = new char *[3];
	this->cmd[0] = strncpy(this->cmd[0], config.cgi_ext[i].bin_path.c_str(), config.cgi_ext[i].bin_path.length());
	std::cout << this->cmd[0] << std::endl;
	this->cmd[1] = strncpy(this->cmd[1], path.c_str(), path.length());
	std::cout << this->cmd[1] << std::endl;
	this->cmd[2] = NULL;
}
