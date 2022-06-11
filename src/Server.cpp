/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: laube <laube@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/11 19:31:50 by laube            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Utils.hpp"
#include "Script.hpp"
#include "event/ConnectionReadEvent.hpp"
#include "event/ConnectionWriteEvent.hpp"
#include "event/TcpStreamEvent.hpp"
#include "http/Handlers.hpp"
#include "http/Request.hpp"
#include "http/RequestLine.hpp"
#include "sock/Connection.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>

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
	this->configList_ = blocks;
    if (blocks.empty()) {
        throw Exception("No server configuration");
    }

    std::vector<sock::TcpStream> streams;
    for (std::vector<Config>::const_iterator it = blocks.begin(); it != blocks.end(); ++it) {
        in_addr address;
        address.s_addr = inet_addr(it->listen.address.c_str());
        if (address.s_addr == INADDR_NONE) {
            std::string msg = "Malformed address: '" + it->listen.address + "'";
            throw Exception(msg.c_str());
        }

        sock::TcpStream s = sock::TcpStream(address, it->listen.port);
        if (std::find(streams.begin(), streams.end(), s) == streams.end()) {
            streams.push_back(s);
        }
    }

    for (std::vector<sock::TcpStream>::const_iterator it = streams.begin(); it != streams.end();
         ++it) {
        sockets_.add(new sock::TcpStream(*it));
    }

    init_tcp_streams();
    configured_ = true;
}

void Server::init_tcp_streams()
{
    for (sock::SocketArray::iterator it = sockets_.begin(); it != sockets_.end(); ++it) {
        sock::TcpStream& s = static_cast<sock::TcpStream&>(**it);

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
 
            sock::SocketArray::iterator socket = sockets_.find(it->fd);
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
                    case sock::TCP_STREAM:
                        events_.push(new event::TcpStreamEvent(*socket));
                        break;
                    case sock::CONNECTION:
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
                const sock::TcpStream& s = static_cast<sock::TcpStream&>(*ev->data());
                accept_connection(s);
                break;
            }
            case event::CONNECTION_READ_EVENT: {
                sock::Connection& c = static_cast<sock::Connection&>(*ev->data());
                receive_data(c);
                break;
            }
            case event::CONNECTION_WRITE_EVENT: {
                sock::Connection& c = static_cast<sock::Connection&>(*ev->data());
				std::vector<Config> resp_configs = getRespConfigs(c, configList_);

        //What do we pass to the following constructor?
        //Response response();
				for(unsigned int i = 0; i < resp_configs.size(); i++){
					std::cout << "CONFIG #" << i << "\n";
					resp_configs[i].print_config();
				}
				//CGI SCRIPT RESPONSE
				if(c.request().requestLine().path().find("cgi-bin", 0) == true){
					//	Script script();
					std::cout << "IN SCRIPT\n\n\n\n\n";
				}

			//if(request = directory){
				//ICI MIK
		//	}



				
				//DIR RESPONSE
				//FILE RESPONSE
                else
                {
                /* Getting the content from an html file:
                * Function getHtml(); should be in Server.hpp.
                * Does Server contain a variable Response?
                */
                std::string line;
                std::string path_prefix = ".";
                std::string full_path = path_prefix + c.request().requestLine().path();
                std::ifstream html_file(full_path);
                std::cout << "THIS IS THE PATH: " << std::endl;
                std::cout << full_path << std::endl;
                if (html_file.is_open())
                {
                    std::cerr << "There was an error when trying to open the html file." << std::endl;
                }
                else
                {
                    std::cout << "=========SHOWING HTML==============" << std::endl;
                    while (getline(html_file, line))
                    {
                    //response.body << line << std::endl;
                    }
                    html_file.close();
                    std::cout << "=========ENDING HTML==============" << std::endl;
                }
                }
                const char* msg = "HTTP/1.0 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n<h1>Hello World Rust is the best "
                                  "language ever made!!!!</h1>\r\n";
								  c.request().print();
                send(c.fd(), msg, strlen(msg), 0);
                close_connection(c);
                break;
            }
        }

        delete ev;
    }
}

void Server::accept_connection(const sock::TcpStream& stream)
{
    sock::Connection* c = new sock::Connection(&stream, MAX_REQUEST_SIZE);

    try {
        c->init();
        sockets_.add(static_cast<sock::Socket*>(c));

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

void Server::receive_data(sock::Connection& c)
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
            http::parse_request_line(c);
            break;
        case http::REQ_HEADERS:
            http::parse_headers(c);
            break;
        case http::REQ_BODY:
            http::parse_body(c);
            break;
        case http::REQ_DONE:
            c.set_write();
            break;
    }
	
}

void Server::close_connection(sock::Connection& c)
{
    sock::SocketArray::iterator socket = sockets_.find(c.fd());
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

std::vector<Config> getRespConfigs(sock::Connection c, std::vector<Config>& configList_){
	std::vector<Config> responseConfigs;
	http::HeaderMap headers = c.request().headers();
	http::HeaderMap::const_iterator it = headers.get("host");
	std::string host = it->second;
	for(unsigned long i = 0; i < configList_.size(); i++){
		if (host == configList_[i].listen.combined){
		responseConfigs.push_back(configList_[i]);
		}
	}
	return responseConfigs;
}
