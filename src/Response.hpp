/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:56:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/16 14:10:54 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include "Config.hpp"
#include "Request.hpp"
#include <string>
#include <unistd.h>
#include <time.h>

#pragma once

class Response
{
public:
    Response(/* args */)
    {
    }

    Response(const Request& request, std::vector<Config>& configs);

    ~Response()
    {
    }

    Config getConfig() const
    {
        return config;
    }

    void setStatusCode(size_t code);

    size_t getStatusCode() const
    {
        return status_code;
    }

    std::string getStatusCodeMsg() const
    {
        return status_code_msg;
    }

    void checkErrorCode();

    void setHtmlBody();
    void setHtmlHeader();
    Method getMethod() {return method;};
    bool has_return_redirect();
    void redirect();
    std::string get_full_path() {return full_path;}
    void remove_file();
    bool method_allowed(Method method);

private:
    void createCodeMsg();
    void buildHeaderString();
    void setContentType();
    void setDate();
    void setHost();
    int generate_autoindex(std::ifstream& requested_file, std::stringstream& body_stream);
    int setAllow();
    Config getSingularConfig(Config og_config);
    Config::Location getSingularLocation(std::vector<Config::Location> locations, bool& has_location);
    void init_response(Config og_config);

public:
    std::string body;
    std::string header;
    std::string full_content;

private:
    Config config;
    Request req;
    size_t body_size;
    size_t header_size;
    size_t status_code;
    Method method;
    std::string status_code_msg;
    std::string root;
    std::string requested_path;
    std::string full_path;
    std::string content_type;
    std::string date_now;
    std::string host;
    std::string allow;
    std::string server;
    std::string location_path;
    std::string file_type;
};
