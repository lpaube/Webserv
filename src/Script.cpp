/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/17 14:48:02 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>

#define BUFFER_SIZE 50

Script::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

// script constructor takes 2 args, a config and a specific request.
Script::Script(const Config& config, const Request& request)
    : envp(NULL),
      cmd(NULL),
      request(request)
{
	std::cout << "IN FORK\n";
    buildCmd(request.path(), config);
    buildEnv(request.method(), config);
    // printEnv();
}

Script::~Script(){

	for (std::size_t i = 0; i < envp_size; i++) {
        delete[] envp[i];
    }
    delete[] envp;
	for (int i = 0; i < 3; i++){
		delete[] cmd[i];
	}
    delete[] cmd;
}

// execute the script and returns the output of the script in a string.
std::string Script::exec()
{
    pid_t id;
    int status;
	int in_file;
	
	if (request.method() == POST) {
		in_file = open("in_file.tmp", O_CREAT | O_RDWR, 0777);
	}
	int out_file = open("out_file.tmp", O_CREAT | O_RDWR, 0777); // Create out file for the output of the script
    id = fork(); // Process to execve the script
    if (id < 0)
        throw Exception("Error fatal, fork");
    if (id == 0) {
		if (request.method() == POST) {
			std::cout << " BODY DATA :\n" <<  request.body().data() << std::endl;
        	write(in_file, request.body().data(), request.body().size());
			close(in_file);
			in_file = open("in_file.tmp", O_RDWR );
        	dup2(in_file, STDIN_FILENO);
		 }
		dup2(out_file, STDOUT_FILENO);
        execve(cmd[0], cmd, envp);
        throw Exception("Error fatal, execve\n\n");
   	}
	else{
        waitpid(id, &status, 0);
	}
    close(out_file);
    if (request.method() == POST) { // delete the in file after script
        close(in_file);
    }
    std::ifstream input_file("out_file.tmp");
    std::string script_output((std::istreambuf_iterator<char>(input_file)),
                              std::istreambuf_iterator<char>());
	remove("in_file.tmp");
	remove("out_file.tmp");
    return script_output;
}

std::string Script::get_ext(const std::string& path)
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
void Script::buildCmd(const std::string& path, const Config& config)
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
    if (found == false){
        throw Exception(msg.c_str());
	}

    // cmd[0] = the name of the program ex: (python or bash)
    //  cmd[1] will be the path where the script is.
    this->cmd = new char*[4];
    this->cmd[0] = new char[config.cgi_ext[i].bin_path.length() + 1]();
	this->cmd[0] = strcpy(this->cmd[0], config.cgi_ext[i].bin_path.c_str());
	this->cmd[1] = new char[path.length() + 1]();
    this->cmd[1] = strcpy(this->cmd[1], path.c_str() + 1);
	this->cmd[2] = new char[1]();
    this->cmd[3] = NULL;
}

// resource :
// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void Script::buildEnv(Method method, const Config& config)
{
    Request::header_iterator it;
    std::stringstream ss;
    std::string add;
    (void)config;

    if (method == POST) {
        // The MIME type of the body of the request, or null if the type is not known
        it = request.find_header("Content-Type");
        if (it != request.headers_end())
            v_env.push_back("CONTENT_TYPE=" + it->second);
    }

    // The length of the request body in bytes made available by the input stream or -1 if the
    // length is not known.
    ss << request.body().size();
    v_env.push_back("CONTENT_LENGTH=" + ss.str() + "");

    // The revision of the CGI specification being used by the server to communicate with the
    // script. It is "CGI/1.1"
    v_env.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // specifies the content types your browser supports. For example, text/xml.
    it = request.find_header("Accept");
    if (it != request.headers_end())
        v_env.push_back("HTTP_ACCEPT=" + it->second);

    // Defines the type of encoding that may be carried out on content returned to the client. For
    // example, compress;q=0.5
    it = request.find_header("Accept-Encoding");
    if (it != request.headers_end())
        v_env.push_back("HTTP_ACCEPT_ENCODING=" + it->second);

    // Used to define which languages you would prefer to receive content in. For example, en;q=0.5.
    // If nothing is returned, no language preference is indicated.
    it = request.find_header("Accept Language");
    if (it != request.headers_end())
        v_env.push_back("HTTP_ACCEPT_LANGUAGE=" + it->second);

    // If the request was forwarded, shows the address and port through of the proxy server.
    it = request.find_header("Forwarded");
    if (it != request.headers_end())
        v_env.push_back("HTTP_FOWARDED=" + it->second);

    // Specifies the Internet host and port number of the resource being requested.
    it = request.find_header("Host");
    if (it != request.headers_end())
        v_env.push_back("HTTP_HOST=" + it->second);

    // Used by a client to identify itself (or its user) to a proxy which requires authentication.
    it = request.find_header("Proxy-Authorization");
    if (it != request.headers_end())
        v_env.push_back("PROXY_AUTHORIZATION=" + it->second);

    // The type and version of the browser the client is using to send the request. For example,
    // Mozilla/1.5
    it = request.find_header("User-Agent");
    if (it != request.headers_end())
        v_env.push_back("HTTP_USER_AGENT=" + it->second);

    // Optionally contains extra path information from the HTTP request that invoked the script,
    // specifying a path to be interpreted by the CGI script. PATH_INFO identifies the resource or
    // sub-resource to be returned by the CGI script, and it is derived from the portion of the URI
    // path following the script name but preceding any query data.
    v_env.push_back("PATH_INFO=" + request.path());

    // The query string that is contained in the request URL after the path.
    v_env.push_back("QUERY_STRING=" + request.query_str());

    // Returns the IP address of the client that sent the request. For HTTP servlets, the value
    // returned is the same as the value of the CGI variable REMOTE_ADDR.
    v_env.push_back("REMOTE_ADDR=" + config.listen.address);

    // The fully-qualified name of the client that sent the request, or the IP address of the client
    // if the name cannot be determined
    it = request.find_header("Host");
    if (it != request.headers_end())
        v_env.push_back("REMOTE_HOST=" + it->second);
    // Returns the name of the HTTP method with which this request was made. For example, GET, POST,
    // or PUT.
    const char* method_name = method_str(request.method());
    std::string method_join("REQUEST_METHOD=");
    method_join += method_name;
    v_env.push_back(method_join);

    // Returns the part of the URL from the protocol name up to the query string in the first line
    // of the HTTP request.
    std::string join("SCRIPT_NAME=");
    join += cmd[0];
    v_env.push_back(join);
    if (config.server_name.size() > 0)
        v_env.push_back("SERVER_NAME=" + config.server_name[0]);

    // Returns the port number on which this request was received. For HTTP servlets, the value
    // returned is the same as the value of the CGI variable SERVER_PORT.
    ss << config.listen.port;
    v_env.push_back("SERVER_PORT=" + ss.str() + "");

    envp_size = v_env.size();
    envp = new char*[envp_size + 1];
    for (std::size_t i = 0; i < envp_size; i++) {
        envp[i] = new char[v_env[i].size() + 1];
        strcpy(envp[i], v_env[i].c_str());
		envp[i][v_env[i].size()] = 0;
    }
    envp[envp_size] = NULL;
}

void Script::printEnv() const
{
    std::size_t i = 0;
    std::cout << "|!|PRINTING CGI ENV|!|\n";
    while (envp[i]) {
        std::cout << envp[i] << "\n";
        i++;
    }
    std::cout << std::flush;
}
