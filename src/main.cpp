/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/05 02:24:04 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ConfigParser.hpp"
#include "Server.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

#include "http/Request.hpp"

std::string first_line(std::string& str)
{
    std::string::size_type pos = str.find("\r\n");
    if (pos == std::string::npos) {
        return "";
    }
    std::string line = str.substr(0, pos + 2);
    str.erase(0, pos + 2);
    return line;
}

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Error, program argument" << std::endl;
        return 1;
    }
    std::string config_file(argv[1]);
    ConfigParser config_parser(config_file);

    config_parser.serverparser[0].generate_fake_config();
    Config fake_config = config_parser.serverparser[0].config;
    fake_config.print_config();

    Server server;
    try {
        //    server.configure(const std::vector<Config> &blocks);
        server.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
