/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <louis-philippe.aube@hotmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 10:52:32 by laube            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfigParser.hpp"
#include "Server.hpp"
#include "Config.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Error, program argument" << std::endl;
        return 1;
    }
    std::string config_file(argv[1]);
    ConfigParser config_parser(config_file);
    // config_parser.serverparser[1].config.print_config();


    // Server server;
    // std::vector<Config> configs;
    // configs.push_back(fake_config);

    // try {
    //     server.configure(configs);
    //     server.run();
    // } catch (const std::exception& ex) {
    //     std::cerr << ex.what() << std::endl;
    // }
}
