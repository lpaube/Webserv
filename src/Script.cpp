/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Script.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/02 18:39:08 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/16 14:24:41 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Script.hpp"
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <sstream>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 50

Script::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

// script constructor takes 2 args, a config and a specific request.
Script::Script(const Config& config, const Request& request)
    : envp(NULL),
      cmd(new char*[3]()),
      request(request)
{
    buildCmd(request.path(), config);
    buildEnv(request.method(), config);
    // printEnv();
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

    // saving STDIN and STDOUT to bring them back to their original FD after
    save[0] = dup(0);
    // TODO: check err
    save[1] = dup(1);
    // TODO: check err

    // if method is POST the script will read from STDIN.
    // Create a file, write the body in it and change it to STDIN.
    if (request.method() == POST) {
        int in_file = open("in_file", O_CREAT | O_APPEND);
        // TODO: check err
        write(in_file, request.body().data(), request.body().size());
        // TODO: check err
        close(in_file);

        dup2(in_file, STDIN_FILENO);
        // TODO: check err
    }
    // Create out file for the output of the script
    int out_file = open("out_file.tmp", O_CREAT | O_RDWR, 0777);
    // TODO: check err
    dup2(out_file, STDOUT_FILENO);
    // TODO: check err

    // Process to execve the script
    id = fork();
    if (id < 0)
        throw Exception("Error fatal, fork");
    if (id == 0) {
        execve(cmd[0], cmd, envp);
        throw Exception("Error fatal, execve\n\n");
    } else
        waitpid(id, &status, 0);
    // free(envp);
    dup2(save[1], 1);
    dup2(save[0], 0); // return stdout to original
    close(out_file);
    /*if (method == http::POST) { // delete the in file after script
        close(in_file);
        remove("in_file");
    }*/
    std::ifstream input_file("out_file.tmp");
    std::string script_output((std::istreambuf_iterator<char>(input_file)),
                              std::istreambuf_iterator<char>());
    remove("out_file.tmp");
    for (std::size_t i = 0; i < envp_size; i++) {
        delete[] envp[i];
    }
    delete[] envp;
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
    if (found == false)
        throw Exception(msg.c_str());

    // cmd[0] = the name of the program ex: (python or bash)
    //  cmd[1] will be the path where the script is.
    this->cmd = new char*[4];
    // Leaks ??
    this->cmd[0] = strdup(config.cgi_ext[i].bin_path.c_str());
    std::cout << this->cmd[0] << std::endl;
    this->cmd[1] = strdup(path.c_str() + 1);
    std::cout << this->cmd[1] << std::endl;
    this->cmd[2] = strdup("\0");
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
    std::cout << "REQUEST LINE : " << request.path();
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

    // Returns the name and version of the protocol the request uses in the following form:
    // protocol/majorVersion.minorVersion. For example, HTTP/1.1
    // v_env.push_back("SERVER_PROTOCOL=HTTP/1.1");

    envp_size = v_env.size();
    envp = new char*[envp_size];
    for (std::size_t i = 0; i < envp_size; i++) {
        envp[i] = new char[v_env[i].size()];
        strcpy(envp[i], v_env[i].c_str());
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
