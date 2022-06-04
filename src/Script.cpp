/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/04 16:19:49 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <string>
#include "http/RequestLine.hpp"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "Utils.hpp"

#define BUFFER_SIZE 50

Script::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

Script::Script(Config& config, http::Request& request) : request(request){
	(void)this->envp;
	this->cmd = new char *[3];
	http::RequestLine	requestline = this->request.requestLine();
	build_cmd(requestline.path(), config);
}

Script::~Script(){
	delete[] cmd;
}

std::string	Script::exec() const{
	pid_t		id;
	int			status;
	int			save[2];
	int			pipe_fd[2];
	int			read_fd[2];

	http::RequestLine request_line = this->request.requestLine();
	http::Method method = request_line.method();

	save[0] = dup(STDIN_FILENO);
	save[1] = dup(STDOUT_FILENO);
	pipe(pipe_fd);
	dup2(pipe_fd[1], STDOUT_FILENO);

	if (method == http::POST){
		pipe(read_fd);
		putstr_fd(this->request.body(), static_cast<std::size_t>(read_fd[1]));
		close(read_fd[1]);
		dup2(read_fd[0], STDIN_FILENO);
		putstr_fd(this->request.body(), static_cast<std::size_t>(pipe_fd[0]));
	}

	id = fork();
	if (id < 0)
		throw Exception("Error fatal, fork");
	if (id == 0){
		execve(cmd[0], cmd, envp);
		throw Exception("Error fatal, execve");
	}
	else
		waitpid(id, &status, 0);
	close (pipe_fd[1]);
	if (method == http::POST){
		dup2(STDIN_FILENO, save[0]);
		close(read_fd[0]);
	}
	dup2(STDOUT_FILENO, save[1]);
	char buf[BUFFER_SIZE];
	std::string	script_ret;
	int	ret = 1;
	while (ret > 0){
		ret = read(pipe_fd[0], buf, BUFFER_SIZE - 2);
		buf[BUFFER_SIZE - 1] = 0;
		script_ret.append(buf);
	}
	close(pipe_fd[0]);
	return script_ret;
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
