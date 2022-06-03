/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/03 18:49:25 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Utils.hpp"
#include "http/Request.hpp"
#include <arpa/inet.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>

Server::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

void Server::configure(const std::vector<Config>& blocks)
{
    sockets_ = SocketArray();

    if (blocks.empty()) {
        throw Exception("No server configuration");
    }

    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        in_addr address;
        address.s_addr = inet_addr(it->listen.address.c_str());
        if (address.s_addr == INADDR_NONE) {
            std::string msg = "Malformed address: '" + it->listen.address + "'";
            throw Exception(msg.c_str());
        }

        if (sockets_.find_match(it->listen.port, address) != sockets_.end()) {
            sockets_.add(it->listen.port, address);
        }
    }
}

static std::string first_line(std::string& str)
{
    std::string::size_type pos = str.find("\r\n");
    if (pos == std::string::npos) {
        return "";
    }
    std::string line = str.substr(0, pos + 2);
    str.erase(0, pos + 2);
    return line;
}

void Server::run()
{
    while (true) {
        int presult = poll(sockets_.pfd_array(), sockets_.pfd_size(), POLL_TIMEOUT);
        if (presult == -1) {
            exception_errno<Exception>("Error while polling sockets: ");
        }

        for (SocketArray::poll_iterator it = sockets_.pfd_begin(); it != sockets_.pfd_end(); ++it) {
            if (it->revents & POLLIN) {
                sockaddr_in addr;
                socklen_t addrlen;
                int conn_fd = accept(it->fd, reinterpret_cast<sockaddr*>(&addr), &addrlen);
                if (conn_fd == -1) {
                    std::cerr << "Connection error: " << strerror(errno) << std::endl;
                }

                timeval timeout = {};
                timeout.tv_sec = CONNECTION_TIMEOUT;
                if (setsockopt(conn_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) == -1) {
                    exception_errno<Exception>("Error while setting socket timeout: ");
                }

                char buf[http::RequestLine::MAX_REQUEST_LINE_SIZE + 1];
                ssize_t bytes = read(conn_fd, buf, http::RequestLine::MAX_REQUEST_LINE_SIZE);
                if (bytes == -1) {
                    std::cerr << "Error reading socket" << std::endl;
                    continue;
                }
                buf[bytes] = 0;

                std::string raw_req(buf);
                std::string line = first_line(raw_req);
                http::RequestLine request_line;
                try {
                    request_line = http::RequestLine(raw_req);
                } catch (std::exception& ex) {
                    std::cerr << ex.what() << std::endl;
                    continue;
                }

                // Read headers
                while (raw_req.find("\r\n\r\n") == std::string::npos &&
                       (bytes = read(conn_fd, buf, http::RequestLine::MAX_REQUEST_LINE_SIZE)) > 0) {
                    buf[bytes] = 0;
                    raw_req.append(buf);
                }

                if (bytes == -1) {
                    std::cerr << "Error reading socket" << std::endl;
                    continue;
                }
            }
        }
    }
}
