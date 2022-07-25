/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/07/25 11:56:27 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include "fd/File.hpp"
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>
#define BUFFER_SIZE 50

#define OUT_TMPFILE "outtmpfile.tmp"

// script constructor takes 2 args, a config and a specific request_.
Script::Script(const Config& config, const Request& request_)
    : ext_found(false),
      v_env_(),
      envp_(NULL),
      cmd_(NULL),
      request_(request_)
{
    build_cmd(request_.path(), config);
    if (ext_found == true)
        build_env(request_.method(), config);
}

Script::~Script()
{
    if (ext_found == true) {
        for (std::size_t i = 0; i < envp_size_; i++) {
            delete[] envp_[i];
        }
        delete[] envp_;
        for (int i = 0; i < 3; i++) {
            delete[] cmd_[i];
        }
        delete[] cmd_;
    }
}

// execute the script and returns the output of the script in a string.
pid_t Script::exec(const std::string& file_name)
{
    pid_t id;
    out_file = open(OUT_TMPFILE, O_CREAT | O_RDWR | O_TRUNC,
                    0777); // Create out file for the output of the script
    if (out_file < 0) {
        throw Request::Exception("Error fatal, open", 500);
    }
    if (request_.method() == POST) {
        in_file = open(file_name.c_str(), O_RDWR, 0777);
        if (in_file < 0) {
            throw Request::Exception("Error fatal, open", 500);
        }
    }
    id = fork(); // Process to execve the script
    if (id < 0)
        throw Request::Exception("Error fatal, fork", 500);
    if (id == 0) {
        if (request_.method() == POST) {
            if (dup2(in_file, STDIN_FILENO) < 0) {
                close(in_file);
                throw Request::Exception("Error fatal, dup2", 500);
            }
        }
        if (dup2(out_file, STDOUT_FILENO) < 0) {
            close(in_file);
            throw Request::Exception("Error fatal, dup2", 500);
        }
        execve(cmd_[0], cmd_, envp_);
        ;
        close(in_file);
        throw Request::Exception("Error fatal, execve\n", 500);
    }
    return id;
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
        throw Request::Exception("Error: No script extention found\n", 402);
    save++;
    ext = path.substr(save);
    return ext;
}

void Script::close_files(int mode)
{
    close(out_file);
    if (mode == 1) {
        close(in_file);
    }
}

// build the cmd_ with the config file and the type of the script in the request_.
void Script::build_cmd(const std::string& path, const Config& config)
{
    std::size_t ext_size = config.cgi_ext.size();
    std::string path_ext = "." + get_ext(path);
    std::size_t i = 0;
    // Find the program linked to the extension in the config file. Throw if not found
    while (i < ext_size) {
        if (path_ext == config.cgi_ext[i].extension) {
            ext_found = true;
            break;
        }
        i++;
    }
    if (ext_found == false)
        return;
    // cmd_[0] = the name of the program ex: (python or bash)
    //  cmd_[1] will be the path where the script is.
    this->cmd_ = new char*[4];
    this->cmd_[0] = new char[config.cgi_ext[i].bin_path.length() + 1]();
    this->cmd_[0] = strcpy(this->cmd_[0], config.cgi_ext[i].bin_path.c_str());
    this->cmd_[1] = new char[path.length() + 1]();
    this->cmd_[1] = strcpy(this->cmd_[1], path.c_str() + 1);
    this->cmd_[2] = new char[1]();
    this->cmd_[3] = NULL;
}

// resource :
// https://www.ibm.com/docs/en/netcoolomnibus/8.1?topic=scripts-environment-variables-in-cgi-script
void Script::build_env(Method method, const Config& config)
{
    Request::header_iterator it;
    std::stringstream ss;
    std::string add;
    (void)config;

    if (method == POST) {
        // The MIME type of the body of the request_, or null if the type is not known
        it = request_.find_header("Content-Type");
        if (it != request_.headers_end())
            v_env_.push_back("CONTENT_TYPE=" + it->second);
    }

    // The length of the request_ body in bytes made available by the input stream or -1 if the
    // length is not known.
    ss << request_.body().size();
    v_env_.push_back("CONTENT_LENGTH=" + ss.str() + "");

    // The revision of the CGI specification being used by the server to communicate with the
    // script. It is "CGI/1.1"
    v_env_.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // specifies the content types your browser supports. For example, text/xml.
    it = request_.find_header("Accept");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_ACCEPT=" + it->second);

    // Defines the type of encoding that may be carried out on content returned to the client. For
    // example, compress;q=0.5
    it = request_.find_header("Accept-Encoding");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_ACCEPT_ENCODING=" + it->second);

    // Used to define which languages you would prefer to receive content in. For example, en;q=0.5.
    // If nothing is returned, no language preference is indicated.
    it = request_.find_header("Accept Language");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_ACCEPT_LANGUAGE=" + it->second);

    // If the request_ was forwarded, shows the address and port through of the proxy server.
    it = request_.find_header("Forwarded");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_FOWARDED=" + it->second);

    // Specifies the Internet host and port number of the resource being request_ed.
    it = request_.find_header("Host");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_HOST=" + it->second);

    // Used by a client to identify itself (or its user) to a proxy which requires authentication.
    it = request_.find_header("Proxy-Authorization");
    if (it != request_.headers_end())
        v_env_.push_back("PROXY_AUTHORIZATION=" + it->second);

    // The type and version of the browser the client is using to send the request_. For example,
    // Mozilla/1.5
    it = request_.find_header("User-Agent");
    if (it != request_.headers_end())
        v_env_.push_back("HTTP_USER_AGENT=" + it->second);

    // Optionally contains extra path information from the HTTP request_ that invoked the script,
    // specifying a path to be interpreted by the CGI script. PATH_INFO identifies the resource or
    // sub-resource to be returned by the CGI script, and it is derived from the portion of the URI
    // path following the script name but preceding any query data.
    v_env_.push_back("PATH_INFO=" + request_.path());

    // The query string that is contained in the request_ URL after the path.
    v_env_.push_back("QUERY_STRING=" + request_.query_str());
    // Returns the IP address of the client that sent the request_. For HTTP servlets, the value
    // returned is the same as the value of the CGI variable REMOTE_ADDR.
    v_env_.push_back("REMOTE_ADDR=" + config.listen.address);

    // The fully-qualified name of the client that sent the request_, or the IP address of the
    // client if the name cannot be determined
    it = request_.find_header("Host");
    if (it != request_.headers_end())
        v_env_.push_back("REMOTE_HOST=" + it->second);
    // Returns the name of the HTTP method with which this request_ was made. For example, GET,
    // POST, or PUT.
    const char* method_name = method_str(request_.method());
    std::string method_join("request__METHOD=");
    method_join += method_name;
    v_env_.push_back(method_join);

    // Returns the part of the URL from the protocol name up to the query string in the first line
    // of the HTTP request_.
    std::string join("SCRIPT_NAME=");
    join += cmd_[0];
    v_env_.push_back(join);
    if (config.server_name.size() > 0)
        v_env_.push_back("SERVER_NAME=" + config.server_name[0]);

    // Returns the port number on which this request_ was received. For HTTP servlets, the value
    // returned is the same as the value of the CGI variable SERVER_PORT.
    ss << config.listen.port;
    v_env_.push_back("SERVER_PORT=" + ss.str() + "");

    envp_size_ = v_env_.size();
    envp_ = new char*[envp_size_ + 1];
    for (std::size_t i = 0; i < envp_size_; i++) {
        envp_[i] = new char[v_env_[i].size() + 1];
        strcpy(envp_[i], v_env_[i].c_str());
        envp_[i][v_env_[i].size()] = 0;
    }
    envp_[envp_size_] = NULL;
}

void Script::print_env() const
{
    std::size_t i = 0;
    std::cout << "|!|PRINTING CGI ENV|!|\n";
    while (envp_[i]) {
        std::cout << envp_[i] << "\n";
        i++;
    }
    std::cout << std::flush;
}

const char* Script::Exception::what() const throw()
{
    return "Error fatal, write returns -1\n";
}
