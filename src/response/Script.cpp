/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/16 15:03:49 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include "Utils.hpp"
#include "http/RequestLine.hpp"
#include "http/Request.hpp"
#include "http/HeaderMap.hpp"
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
#include <stdlib.h>
#include <stdio.h>
#include "http/Method.hpp"

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
	http::Method method = requestline.method();

    buildCmd(requestline.path(), config);
	buildEnv(method, config);
	//printEnv();
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
    // Create out file for the output of the script
    int out_file = open("out_file.tmp", O_CREAT | O_RDWR);
	chmod("out_file.tmp", 0777);
	dup2(out_file, STDOUT_FILENO);

   // Process to execve the script
    id = fork();
    if (id < 0)
        throw Exception("Error fatal, fork");
    if (id == 0) {
        execve(cmd[0], cmd, envp);
        throw Exception("Error fatal, execve\n\n");
    } else
        waitpid(id, &status, 0);
	//free(envp);
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
	for (std::size_t i = 0; i < envp_size; i++){
		delete[] envp[i];
	}
	delete[] envp;
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

//resource : https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void	Script::buildEnv(http::Method& method, Config& config){
	http::HeaderMap header = this->request.headers();
	http::HeaderMap::const_iterator it;
	std::stringstream ss;
	std::string add;
	(void)config;
	
	if (method == http::POST){
		//The MIME type of the body of the request, or null if the type is not known
		it = header.get("Content-Type");
		if (it != header.end())
			v_env.push_back("CONTENT_TYPE=" + it->second);
	}
	
	//The length of the request body in bytes made available by the input stream or -1 if the length is not known.
	ss << this->request.content_length();
	v_env.push_back("CONTENT_LENGTH=" + ss.str() + "");

	//The revision of the CGI specification being used by the server to communicate with the script. It is "CGI/1.1"
	v_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

	//specifies the content types your browser supports. For example, text/xml.
	it = header.get("Accept");
	if (it != header.end())
		v_env.push_back("HTTP_ACCEPT=" + it->second);

	//Defines the type of encoding that may be carried out on content returned to the client. For example, compress;q=0.5
	it = header.get("Accept-Encoding");
	if (it != header.end())
		v_env.push_back("HTTP_ACCEPT_ENCODING=" + it->second);

	//Used to define which languages you would prefer to receive content in. For example, en;q=0.5. If nothing is returned, no language preference is indicated.
	it = header.get("Accept Language");
	if (it != header.end())
		v_env.push_back("HTTP_ACCEPT_LANGUAGE=" + it->second);
	
	//If the request was forwarded, shows the address and port through of the proxy server. 
	it = header.get("Forwarded");
	if (it != header.end())
		v_env.push_back("HTTP_FOWARDED=" + it->second);

	//Specifies the Internet host and port number of the resource being requested.
	it = header.get("Host");
	if (it != header.end())
		v_env.push_back("HTTP_HOST=" + it->second);

	//Used by a client to identify itself (or its user) to a proxy which requires authentication.
	it = header.get("Proxy-Authorization");
	if (it != header.end())
		v_env.push_back("PROXY_AUTHORIZATION=" + it->second);

	//The type and version of the browser the client is using to send the request. For example, Mozilla/1.5
	it = header.get("User-Agent");
	if (it !=  header.end())
		v_env.push_back("HTTP_USER_AGENT=" + it->second);

	//Optionally contains extra path information from the HTTP request that invoked the script, specifying a path to be interpreted by the CGI script. PATH_INFO identifies the resource or sub-resource to be returned by the CGI script, and it is derived from the portion of the URI path following the script name but preceding any query data.	
	v_env.push_back("PATH_INFO=" + request.requestLine().path());

	//The query string that is contained in the request URL after the path.
	v_env.push_back("QUERY_STRING=" + request.requestLine().query());
	
	//Returns the IP address of the client that sent the request. For HTTP servlets, the value returned is the same as the value of the CGI variable REMOTE_ADDR.
	v_env.push_back("REMOTE_ADDR=" + config.listen.address);
	
	//The fully-qualified name of the client that sent the request, or the IP address of the client if the name cannot be determined
	it = header.get("Host");
	if (it != header.end())
		v_env.push_back("REMOTE_HOST=" + it->second);
	//Returns the name of the HTTP method with which this request was made. For example, GET, POST, or PUT.
	std::cout << "REQUEST LINE : " << request.requestLine().path();
	const char *method_name = method_str(request.requestLine().method());
	std::string method_join("REQUEST_METHOD=");
	method_join += method_name;
	v_env.push_back(method_join);

	//Returns the part of the URL from the protocol name up to the query string in the first line of the HTTP request.
	std::string join("SCRIPT_NAME=");
	join += cmd[0];
	v_env.push_back(join);
	if (config.server_name.size() > 0)
		v_env.push_back("SERVER_NAME=" + config.server_name[0]);

	//Returns the port number on which this request was received. For HTTP servlets, the value returned is the same as the value of the CGI variable SERVER_PORT.
	ss << config.listen.port;
	v_env.push_back("SERVER_PORT=" + ss.str() + "");
	
	//Returns the name and version of the protocol the request uses in the following form: protocol/majorVersion.minorVersion. For example, HTTP/1.1
	//v_env.push_back("SERVER_PROTOCOL=HTTP/1.1");

	envp_size = v_env.size();
	envp = new char *[envp_size];
	for (std::size_t i = 0; i < envp_size; i++){
		envp[i] = new char[v_env[i].size()];
		strcpy(envp[i], v_env[i].c_str());
	}
	envp[envp_size] = NULL;
}

void	Script::printEnv() const{
	std::size_t i = 0;
	std::cout << "|!|PRINTING CGI ENV|!|\n";
	while (envp[i]){
		std::cout << envp[i] << "\n";
		i++;
	}
	std::cout << std::flush;
}
