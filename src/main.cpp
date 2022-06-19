#include "ConfigParser.hpp"
#include "Server.hpp"
#include <exception>
#include <iostream>

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cout << "Error, program argument" << std::endl;
        return 1;
    }
    std::string config_file(argv[1]);
    ConfigParser config_parser(config_file);

    std::vector<Config> configs;
    for (unsigned int i = 0; i < config_parser.nbserver(); i++) {
        configs.push_back(config_parser.serverparser[i].config);
    }

    try {
        Server server(configs);
        server.run();
    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}
