#include "Config.hpp"

Config& Config::operator=(const Config::Location& other_location)
{
    error_page = other_location.error_page;
    client_max_body_size = other_location.client_max_body_size;
    limit_except = other_location.limit_except;
    root = other_location.root;
    autoindex = other_location.autoindex;
    index = other_location.index;
    cgi_ext = other_location.cgi_ext;
    return_redirect = other_location.return_redirect;
    return *this;
}

void Config::Location::print_location() const
{
    // location_path
    std::cout << "===== location_path =====" << std::endl;
    std::cout << location_path << std::endl;
    // error_page
    std::cout << "===== error_page =====" << std::endl;
    int i = 0;
    for (std::vector<Error_page>::const_iterator it = error_page.begin(); it != error_page.end();
         ++it, ++i) {
        std::cout << "--- error_page: " << i << " ---" << std::endl;
        for (std::vector<int>::const_iterator it_code = it->code.begin(); it_code != it->code.end();
             ++it_code) {
            std::cout << "code: " << *it_code << std::endl;
        }
        std::cout << "uri: " << it->uri << std::endl;
    }
    std::cout << std::endl;

    // client_max_body_size
    std::cout << "===== client_max_body_size =====" << std::endl;
    std::cout << "max_body_size: " << client_max_body_size << std::endl;
    std::cout << std::endl;

    // return
    std::cout << "===== return =====" << std::endl;
    std::cout << "code: " << return_redirect.code << std::endl;
    std::cout << "redirection url: " << return_redirect.url << std::endl;
    std::cout << std::endl;

    // root
    std::cout << "===== root =====" << std::endl;
    std::cout << "root: " << root << std::endl;
    std::cout << std::endl;

    // limit_except
    std::cout << "===== limit_except =====" << std::endl;
    for (std::vector<std::string>::const_iterator it = limit_except.begin();
         it != limit_except.end(); ++it) {
        std::cout << "allowed method: " << *it << std::endl;
    }
    std::cout << std::endl;

    // autoindex
    std::cout << "===== autoindex =====" << std::endl;
    std::cout << "autoindex: " << autoindex << std::endl;
    std::cout << std::endl;

    // index
    std::cout << "===== index =====" << std::endl;
    for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it) {
        std::cout << "index: " << *it << std::endl;
    }
    std::cout << std::endl;

    // cgi_ext
    std::cout << "===== cgi_ext =====" << std::endl;
    i = 0;
    for (std::vector<Cgi_ext>::const_iterator it = cgi_ext.begin(); it != cgi_ext.end(); ++it) {
        std::cout << "---cgi_ext: " << i << "---" << std::endl;
        std::cout << "extension: " << it->extension << std::endl;
        std::cout << "binary path: " << it->bin_path << std::endl;
    }
    std::cout << std::endl;
}

void Config::print_config() const
{
    // listen
    std::cout << "===== listen =====" << std::endl;
    std::cout << "combined: " << listen.combined << std::endl;
    std::cout << "address: " << listen.address << std::endl;
    std::cout << "port: " << listen.port << std::endl;
    std::cout << std::endl;

    // server_name
    std::cout << "===== server_name =====" << std::endl;
    for (std::vector<std::string>::const_iterator it = server_name.begin(); it != server_name.end();
         ++it) {
        std::cout << "server name: " << *it << std::endl;
    }
    std::cout << std::endl;

    // error_page
    std::cout << "===== error_page =====" << std::endl;
    int i = 0;
    for (std::vector<Error_page>::const_iterator it = error_page.begin(); it != error_page.end();
         ++it, ++i) {
        std::cout << "--- error_page: " << i << " ---" << std::endl;
        for (std::vector<int>::const_iterator it_code = it->code.begin(); it_code != it->code.end();
             ++it_code) {
            std::cout << "code: " << *it_code << std::endl;
        }
        std::cout << "uri: " << it->uri << std::endl;
    }
    std::cout << std::endl;

    // client_max_body_size
    std::cout << "===== client_max_body_size =====" << std::endl;
    std::cout << "max_body_size: " << client_max_body_size << std::endl;
    std::cout << std::endl;

    // return
    std::cout << "===== return =====" << std::endl;
    std::cout << "code: " << return_redirect.code << std::endl;
    std::cout << "redirection url: " << return_redirect.url << std::endl;
    std::cout << std::endl;

    // root
    std::cout << "===== root =====" << std::endl;
    std::cout << "root: " << root << std::endl;
    std::cout << std::endl;

    // limit_except
    std::cout << "===== limit_except =====" << std::endl;
    for (std::vector<std::string>::const_iterator it = limit_except.begin();
         it != limit_except.end(); ++it) {
        std::cout << "allowed method: " << *it << std::endl;
    }
    std::cout << std::endl;

    // autoindex
    std::cout << "===== autoindex =====" << std::endl;
    std::cout << "autoindex: " << autoindex << std::endl;
    std::cout << std::endl;

    // index
    std::cout << "===== index =====" << std::endl;
    for (std::vector<std::string>::const_iterator it = index.begin(); it != index.end(); ++it) {
        std::cout << "index: " << *it << std::endl;
    }
    std::cout << std::endl;

    // cgi_ext
    std::cout << "===== cgi_ext =====" << std::endl;
    i = 0;
    for (std::vector<Cgi_ext>::const_iterator it = cgi_ext.begin(); it != cgi_ext.end(); ++it) {
        std::cout << "---cgi_ext: " << i << "---" << std::endl;
        std::cout << "extension: " << it->extension << std::endl;
        std::cout << "binary path: " << it->bin_path << std::endl;
    }
    std::cout << std::endl;

    // Location
    int elem = 0;
    for (std::vector<Location>::const_iterator loc_it = location.begin(); loc_it != location.end();
         ++loc_it, ++elem) {
        std::cout << "==============================" << std::endl;
        std::cout << "===== LOCATION " << elem << " ============" << std::endl;
        std::cout << "==============================" << std::endl;
        loc_it->print_location();
    }
    std::cout << std::endl;
}

Config::Location& Config::Location::operator=(const Config& config)
{
    error_page = config.error_page;
    client_max_body_size = config.client_max_body_size;
    limit_except = config.limit_except;
    return_redirect = config.return_redirect;
    root = config.root;
    autoindex = config.autoindex;
    index = config.index;
    cgi_ext = config.cgi_ext;
    return *this;
}
