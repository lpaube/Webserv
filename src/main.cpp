/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 14:22:28 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../include/ConfigParser.hpp"

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Error, program argument\n" << std::endl;
		return 1;
	}
	std::string	config_file(argv[1]);
	ConfigParser config_parser(config_file);

  try {
    config_parser.serverparser[0].parse_config_vars();
  }
  catch (char const* s)
  {
    std::cout << s << std::endl;
  }
  config_parser.serverparser[0].config.print_config();

  
   /* int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8000);

    int res = bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    (void)res;

    listen(sock, 10);

    for (;;) {
        int connfd;
        struct sockaddr_in addr;
        socklen_t addrlen;

        connfd = accept(sock, (struct sockaddr*)&addr, &addrlen);

        char buff[2049] = {};
        ssize_t n;

        while ((n = read(connfd, buff, 2048)) > 0) {
            buff[n] = 0;
            std::cout << buff;

            if (strncmp(buff + n - 4, "\r\n\r\n", 4) == 0) {
                break;
            }
        }
        std::cout << std::endl;

        snprintf(buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello World Rust is the best language ever made");
        write(connfd, buff, strlen(buff));
        close(connfd);
    }*/
}
