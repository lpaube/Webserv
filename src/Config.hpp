#pragma once
#include "LocationContent.hpp"
#include <iostream>
#include <string>

class Config
{
public:
    struct Listen {
        std::string combined;
        std::string address;
        // port is initialized to -1
        int port;
    };

    struct Error_page {
        std::vector<size_t> code;
        std::string uri;
    };

    struct Return_redirect {
        // code is initialized to -1
        int code;
        std::string url;
    };

    struct Cgi_ext {
        std::string extension;
        std::string bin_path;
    };

    class Location
    {
    public:
        std::string location_match;
        std::vector<Error_page> error_page;
        unsigned long client_max_body_size; // Represented in bytes but only takes m in config
        std::vector<std::string> limit_except;
        Return_redirect return_redirect;
        std::string root;
        bool autoindex;
        std::vector<std::string> index;
        std::vector<Cgi_ext> cgi_ext;

        void print_location();
    };

public:
    Listen listen;
    std::vector<std::string> server_name;
    std::vector<Error_page> error_page;
    unsigned long client_max_body_size; // Represented in bytes but only takes m in config
    std::vector<std::string> limit_except;
    Return_redirect return_redirect;
    std::string root;
    bool autoindex;
    std::vector<std::string> index;
    std::vector<Cgi_ext> cgi_ext;
    std::vector<Location> location;

    void print_config();
};
