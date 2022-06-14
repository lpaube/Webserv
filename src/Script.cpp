/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/11 21:05:51 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include "Utils.hpp"
#include "http/RequestLine.hpp"
#include "http/Request.hpp"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <sstream>

#define BUFFER_SIZE 50

Script::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

// script constructor takes 2 args, a config and a specific request.
Script::Script(Config& config, http::Request& request)
    : request(request)
{
    (void)this->envp;
    this->cmd = new char*[3];
    http::RequestLine requestline = this->request.requestLine();
    buildCmd(requestline.path(), config);
}

Script::~Script()
{
    delete[] cmd;
}

// execute the script and returns the output of the script in a string.
std::string Script::exec()
{
   pid_t id;
    int status;
    int save[2];

    http::RequestLine request_line = this->request.requestLine();
    http::Method method = request_line.method();

    // saving STDIN and STDOUT to bring them back to their original FD after
    save[0] = dup(0);
    save[1] = dup(1);

	// if method is POST the script will read from STDIN.
    // Create a file, write the body in it and change it to STDIN.
    std::size_t in_file;
    if (method == http::POST) {
        in_file = static_cast<std::size_t>(open("in_file", O_CREAT | O_APPEND));
        putstr_fd(this->request.body(), in_file);
        close(in_file);
        in_file = static_cast<std::size_t>(open("in_file", O_RDONLY));
        dup2(in_file, STDIN_FILENO);
    }
	buildEnv(method);
    // Create out file for the output of the script
    int out_file = open("out_file.tmp", O_CREAT | O_RDWR);
	chmod("out_file.tmp", 0777);
	dup2(out_file, STDOUT_FILENO);

   // Process to execve the script
    id = fork();
    if (id < 0)
        throw Exception("Error fatal, fork");
    if (id == 0) {
        execve(cmd[0], cmd, cmd);
        throw Exception("Error fatal, execve\n\n");
    } else
    {
        waitpid(id, &status, 0);
    }
	dup2(save[1], 1);
	dup2(save[0], 0);//return stdout to original
	close(out_file);
    /*if (method == http::POST) { // delete the in file after script
        close(in_file);
        remove("in_file");
    }*/
	std::ifstream input_file("out_file.tmp");
	std::string script_output((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
	remove("out_file.tmp");
    return script_output;
}

std::string Script::get_ext(std::string& path)
{
    unsigned long len = path.length();
    unsigned long save = 0;
    unsigned long i = 0;
    std::string ext;
    while (i < len) {
        if (path[i] == '/')
            save = i;
        i++;
    }
    while (save < len) {
        if (path[save] == '.')
            break;
        else
            save++;
    }
    if (save == len)
        throw Exception("Error: No script extention found");
    save++;
    ext = path.substr(save);
    return ext;
}

// build the cmd with the config file and the type of the script in the request.
void Script::buildCmd(std::string path, Config& config)
{
    std::size_t ext_size = config.cgi_ext.size();
    std::string path_ext = "." + get_ext(path);
    bool found = false;
    std::size_t i = 0;
    // Find the program linked to the extension in the config file. Throw if not found
    while (i < ext_size) {
        if (path_ext == config.cgi_ext[i].extension) {
            found = true;
            break;
        }
        i++;
    }
    std::string msg = "Error: script extension ." + path_ext;
    if (found == false)
        throw Exception(msg.c_str());

    // cmd[0] = the name of the program ex: (python or bash)
    //  cmd[1] will be the path where the script is.
    this->cmd = new char*[4];
    this->cmd[0] = strdup(config.cgi_ext[i].bin_path.c_str());
    std::cout << this->cmd[0] << std::endl;
    this->cmd[1] = strdup(path.c_str() + 1);
    std::cout << this->cmd[1] << std::endl;
    this->cmd[2] = strdup("\0");
	this->cmd[3] = NULL;
}

void	Script::buildEnv(http::Method& method){
	if (method == http::POST){
	}
}
