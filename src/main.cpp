/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/02 16:07:39 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
#include "../include/ConfigParser.hpp"

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

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Error, program argument\n" << std::endl;
		return 1;
	}
	std::string	config_file(argv[1]);
	ConfigParser config_parser(config_file);

  config_parser.serverparser[0].generate_fake_config();
  Config fake_config = config_parser.serverparser[0].config;
  fake_config.print_config();


  
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8000);

    int res = bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    (void)res;

    int enable = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
    setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(enable));

    // fcntl(sock, F_SETFL, O_NONBLOCK);

    listen(sock, 10);

    while (true) {
        int connfd;
        struct sockaddr_in addr;
        socklen_t addrlen;

        connfd = accept(sock, (struct sockaddr*)&addr, &addrlen);

        struct timeval timeout;
        timeout.tv_sec = 5;
        timeout.tv_usec = 0;
        setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        // fcntl(connfd, O_NONBLOCK);

        // Read request line
        char buff[http::RequestLine::MAX_REQUEST_LINE_SIZE + 1] = {};
        ssize_t n = read(connfd, buff, http::RequestLine::MAX_REQUEST_LINE_SIZE);
        if (n == -1) {
            // error or timeout
            exit(1);
        }
        buff[n] = 0;

        std::string raw_str(buff);
        std::string line = first_line(raw_str);
        http::RequestLine request_line;
        try {
            request_line = http::RequestLine(line);
        } catch (std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }

        // Read headers
        while (raw_str.find("\r\n\r\n") == std::string::npos &&
               (n = read(connfd, buff, http::RequestLine::MAX_REQUEST_LINE_SIZE)) > 0) {
            buff[n] = 0;
            raw_str.append(buff);
        }

        if (n == -1) {
            if (errno == EAGAIN) {
                // timeout
            }
        }

        try {
            http::Request request(request_line, raw_str);

            raw_str.erase(0, raw_str.find("\r\n\r\n") + 4);

            // TODO: handle chunked request
            ssize_t bytes_left = request.content_length();
            bytes_left -= (ssize_t)raw_str.length();

            // read body
            while (bytes_left > 0 &&
                   (n = read(connfd, buff,
                             (std::size_t)bytes_left < http::RequestLine::MAX_REQUEST_LINE_SIZE
                                 ? (std::size_t)bytes_left
                                 : http::RequestLine::MAX_REQUEST_LINE_SIZE)) > 0) {
                bytes_left -= n;
                buff[n] = 0;
                raw_str.append(buff);
            }
            request.set_body(raw_str);
            request.print();
        } catch (std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }

        snprintf(buff, sizeof(buff),
                 "HTTP/1.0 200 OK\r\n\r\n<h1>Hello World Rust is the best language ever made</h1>");
        write(connfd, buff, std::strlen(buff));
        close(connfd);
    }

    close(sock);
}
