/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 18:01:24 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Config.hpp"
#include "ConfigParser.hpp"
#include "Server.hpp"

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Error, program argument" << std::endl;
        return 1;
    }
    std::string config_file(argv[1]);
    ConfigParser config_parser(config_file);
    config_parser.serverparser[0].config.print_config();


     Server server;
     std::vector<Config> configs;
	 for (unsigned int i = 0; i < config_parser.nbServer(); i++){
		 configs.push_back(config_parser.serverparser[i].config);
	 }
     try {
         server.configure(configs);
         server.run();
     } catch (const std::exception& ex) {
         std::cerr << ex.what() << std::endl;
     }
}
