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

#include "Config.hpp"
#include "Request.hpp"
#include <string>
#include <unistd.h>
#include <time.h>

#pragma once

class Response
{
public:
    Response(const Request& request, const std::vector<Config>& response_configs);
    Config get_config() const;
    int get_status_code() const;
    std::string get_status_code_msg() const;
    std::string get_location_path() const;
    std::string get_full_path() const;
    Method get_method() const;
    bool has_return_redirect() const;
    bool method_allowed(Method method) const;

    void set_status_code(int code);
    void set_html_body();
    void set_html_header();

    void check_error_code();
    void redirect();
    void remove_file();

private:
    void create_code_msg();
    void build_header_string();
    void set_content_type();
    void set_date();
    void set_host();
    int set_allow();
    int generate_autoindex(std::ifstream& requested_file, std::stringstream& body_stream);
    void generate_singular_config();
    Config::Location generate_singular_location(const std::vector<Config::Location>& locations, bool& has_location);

public:
    std::string body;
    std::string header;
    std::string full_content;

private:
    Config config_;
    Request req_;
    size_t body_size_;
    size_t header_size_;
    int status_code_;
    Method method_;
    std::string status_code_msg_;
    std::string root_;
    std::string requested_path_;
    std::string full_path_;
    std::string content_type_;
    std::string date_now_;
    std::string host_;
    std::string allow_;
    std::string server_;
    std::string location_path_;
    std::string file_type_;
};
