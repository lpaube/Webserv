/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/06 20:00:50 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Connection.hpp"
#include "Utils.hpp"
#include "event/ConnectionReadEvent.hpp"
#include "event/ConnectionWriteEvent.hpp"
#include "event/TcpStreamEvent.hpp"
#include "http/Request.hpp"
#include "http/RequestLine.hpp"
#include <algorithm>
#include <arpa/inet.h>
#include <cstdio>
#include <sys/errno.h>
#include <sys/time.h>
#include <unistd.h>

Server::Exception::Exception(const char* msg)
    : ExceptionBase(msg)
{
}

Server::Server()
    : configured_(false)
{
}

void Server::configure(const std::vector<Config>& blocks)
{
    sockets_.clear();

    if (blocks.empty()) {
        throw Exception("No server configuration");
    }

    std::vector<TcpStream> streams;
    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        in_addr address;
        address.s_addr = inet_addr(it->listen.address.c_str());
        if (address.s_addr == INADDR_NONE) {
            std::string msg = "Malformed address: '" + it->listen.address + "'";
            throw Exception(msg.c_str());
        }

        TcpStream s = TcpStream(address, it->listen.port);
        if (std::find(streams.begin(), streams.end(), s) == streams.end()) {
            streams.push_back(s);
        }
    }

    for (std::vector<TcpStream>::const_iterator it = streams.begin(); it != streams.end(); ++it) {
        sockets_.add(new TcpStream(*it));
    }

    init_tcp_streams();
    configured_ = true;
}

void Server::init_tcp_streams()
{
    for (SocketArray::iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        TcpStream& s = static_cast<TcpStream&>(**it);

        s.init();
        s.bind();
        s.listen();

        pollfd pfd;
        pfd.fd = s.fd();
        pfd.events = POLLIN;
        pfd.revents = 0;
        pfds_.push_back(pfd);
    }
}

void Server::run()
{
    if (!configured_) {
        throw Exception("Server is not configured");
    }

    while (true) {
        int n_ready = poll(pfds_.data(), pfds_.size(), POLL_TIMEOUT);
        if (n_ready == -1) {
            exception_errno<Exception>("Error while polling sockets: ");
        }

        for (std::vector<pollfd>::iterator it = pfds_.begin(); it != pfds_.end() && n_ready > 0;
             ++it) {
            if (n_ready == 0) {
                break;
            }

            bool found = false;

            SocketArray::iterator socket = sockets_.find(it->fd);
            if (socket == sockets_.end()) {
                // This should never happen
                std::cerr << "Did not find socket: " << it->fd << std::endl;
                continue;
            }

            if (it->revents & (POLLERR | POLLHUP | POLLNVAL)) {
                // If there was an error, add POLLIN | POLLOUT to catch it in one of the handlers
                it->revents |= POLLIN | POLLOUT;
            }
            if ((it->revents & POLLIN) && (*socket)->read()) {
                found = true;

                switch ((*socket)->type()) {
                    case TCP_STREAM:
                        events_.push(new event::TcpStreamEvent(*socket));
                        break;
                    case CONNECTION:
                        events_.push(new event::ConnectionReadEvent(*socket));
                        break;
                }
            }
            if ((it->revents & POLLOUT) && !(*socket)->read()) {
                found = true;
                events_.push(new event::ConnectionWriteEvent(*socket));
            }

            if (found) {
                --n_ready;
            }
        }

        process_event_queue();
    }
}

void Server::process_event_queue()
{
    while (!events_.empty()) {
        event::Event* ev = events_.pop();

        switch (ev->type()) {
            case event::TCP_STREAM_EVENT: {
                const TcpStream& s = static_cast<TcpStream&>(*ev->data());
                accept_connection(s);
                break;
            }
            case event::CONNECTION_READ_EVENT: {
                Connection& c = static_cast<Connection&>(*ev->data());
                receive_data(c);
                break;
            }
            case event::CONNECTION_WRITE_EVENT: {
                Connection& c = static_cast<Connection&>(*ev->data());
                const char* msg = "HTTP/1.0 200 OK\r\n\r\n<h1>Hello World Rust is the best "
                                  "language ever made</h1>\r\n";
                send(c.fd(), msg, strlen(msg), 0);
                close_connection(c);
                break;
            }
        }

        delete ev;
    }
}

void Server::accept_connection(const TcpStream& stream)
{
    Connection* c = new Connection(&stream, MAX_REQUEST_SIZE);

    try {
        c->init();
        sockets_.add(static_cast<Socket*>(c));

        pollfd pfd;
        pfd.fd = c->fd();
        pfd.events = POLLIN | POLLOUT;
        pfd.revents = 0;
        pfds_.push_back(pfd);
    } catch (const std::exception& ex) {
        delete c;
        std::cerr << ex.what() << std::endl;
    }
}

void Server::receive_data(Connection& c)
{
    char buf[BUFFER_SIZE];
    ssize_t prev_read = 0;
    ssize_t total_read = 0;
    bool error = false;

    while (true) {
        ssize_t n = recv(c.fd(), buf, BUFFER_SIZE, 0);

        // EAGAIN / EWOULDBLOCK
        if (prev_read == BUFFER_SIZE && n < 0) {
            break;
        }

        if (n < 0) {
            error = true;
            break;
        }

        prev_read = n;
        total_read += n;
        c.append_data(buf, buf + n);

        if ((size_t)n < BUFFER_SIZE || total_read > MAX_REQUEST_SIZE) {
            break;
        }
    }

    if (error) {
        close_connection(c);
        return;
    }

    // Client closed connection
    if (total_read == 0) {
        close_connection(c);
        return;
    }

    // Request too big
    if (total_read > MAX_REQUEST_SIZE) {
        close_connection(c);
        return;
    }

    switch (c.request_state()) {
        case http::REQ_LINE:
            parse_http_request_line(c);
            break;
        case http::REQ_HEADERS:
            parse_http_headers(c);
            break;
        case http::REQ_BODY:
            parse_http_body(c);
            break;
        case http::REQ_DONE:
            c.set_write();
            break;
    }
}

void Server::close_connection(Connection& c)
{
    SocketArray::iterator socket = sockets_.find(c.fd());
    if (socket == sockets_.end()) {
        std::cerr << "Can't find connection" << std::endl;
        return;
    }

    for (std::vector<pollfd>::iterator pfd = pfds_.begin(); pfd != pfds_.end(); ++pfd) {
        if (pfd->fd == c.fd()) {
            pfds_.erase(pfd);
            break;
        }
    }

    sockets_.erase(socket);
}

void Server::parse_http_request_line(Connection& c)
{
    http::RequestLine line;
    bool error = false;
    try {
        line = http::RequestLine(c.buffer());
        c.request() = http::Request(line);
    } catch (const std::exception& ex) {
        error = true;
        std::cerr << ex.what() << std::endl;
    }

    if (error) {
        // TODO: bad request
        return;
    }

    c.set_request_state(http::REQ_HEADERS);
    parse_http_headers(c);
}

void Server::parse_http_headers(Connection& c)
{
    const char* sep = "\r\n";
    const size_t sep_size = 2;

    Buffer& buf = c.buffer();
    const char* ptr;
    while ((ptr = buf.find(sep, sep_size)) != NULL) {
        if (ptr == buf.cursor()) {
            c.set_request_state(http::REQ_BODY);
            buf.advance_cursor(2);
            break;
        }

        try {
            http::Header header(get_next_word(buf, sep, sep_size));
            c.request().add_header(header);
        } catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
        }
    }
    buf.erase_to_cursor();

    if (c.request_state() == http::REQ_BODY) {
        parse_http_body(c);
    }
}

void Server::parse_http_body(Connection& c)
{
    c.set_write();
}
