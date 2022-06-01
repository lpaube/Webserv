/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/01 02:04:39 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <iostream>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http/Request.hpp"

int main()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(8000);

    int res = bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr));
    (void)res;

    listen(sock, 10);

    while (true) {
        int connfd;
        struct sockaddr_in addr;
        socklen_t addrlen;

        connfd = accept(sock, (struct sockaddr*)&addr, &addrlen);

        char buff[2049] = {};
        ssize_t n;

        std::string req_str;
        while ((n = read(connfd, buff, 10)) > 0) {
            buff[n] = 0;
            req_str.append(buff);

            if (req_str.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }

        try {
            http::Request request(req_str);

            req_str.erase(0, req_str.find("\r\n\r\n") + 4);
            ssize_t bytes_left = request.content_length();
            bytes_left -= (ssize_t)req_str.length();
            while (bytes_left > 0 &&
                   (n = read(connfd, buff, bytes_left < 10 ? (size_t)bytes_left : 10)) > 0) {
                bytes_left -= n;
                buff[n] = 0;
                req_str.append(buff);
            }
            request.set_body(req_str);
            request.print();
        } catch (std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }

        snprintf(buff, sizeof(buff),
                 "HTTP/1.0 200 OK\r\n\r\nHello World Rust is the best language ever made");
        write(connfd, buff, std::strlen(buff));
        close(connfd);
        break;
    }

    close(sock);
}
