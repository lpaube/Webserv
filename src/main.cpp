/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:21:49 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:41:13 by mleblanc         ###   ########.fr       */
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
#include <sys/_types/_socklen_t.h>
#include <sys/_types/_ssize_t.h>
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
    servaddr.sin_port = htons(9000);

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

        std::string req;
        while ((n = read(connfd, buff, 2048)) > 0) {
            buff[n] = 0;
            std::cout << buff;
            req.append(buff);

            if (strncmp(buff + n - 4, "\r\n\r\n", 4) == 0) {
                break;
            }
        }

        try {
            http::Request r(req);
        } catch (std::exception& ex) {
            std::cerr << ex.what() << std::endl;
        }

        snprintf(buff, sizeof(buff), "HTTP/1.0 200 OK\r\n\r\nHello World Rust is the best language ever made");
        write(connfd, buff, strlen(buff));
        close(connfd);
    }

    close(sock);
}
