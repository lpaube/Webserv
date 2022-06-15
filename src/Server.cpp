/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:55 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/15 12:56:35 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Utils.hpp"
#include "response/Script.hpp"
#include "event/ConnectionReadEvent.hpp"
#include "event/ConnectionWriteEvent.hpp"
#include "event/TcpStreamEvent.hpp"
#include "http/Handlers.hpp"
#include "http/Request.hpp"
#include "http/RequestLine.hpp"
#include "sock/Connection.hpp"
#include "response/Response.hpp"
#include "StatusCodes.hpp"
#include <arpa/inet.h>
#include <cstdio>
#include <sys/time.h>
#include <unistd.h>
#include <fstream>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
 # include <sys/errno.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/wait.h>
# include <stdio.h>
# include <unistd.h>
# include <signal.h>
# include <stdlib.h>
# include <limits.h>
# include <dirent.h>
# include <stdbool.h>

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
	const char* msg;
	  while (!events_.empty()) {
        event::Event* ev = events_.pop();

        switch (ev->type()) {
            case event::TCP_STREAM_EVENT: {
				std::cout <<	"|!|IN_CONNECTION_STREAM_EVENT|!|" << std::endl;
                const sock::TcpStream& s = static_cast<sock::TcpStream&>(*ev->data());
                accept_connection(s);
                break;
            }
            case event::CONNECTION_READ_EVENT: {
				std::cout << "|!|IN_CONNECTION_READ_EVENT|!|" << std::endl;
                sock::Connection& c = static_cast<sock::Connection&>(*ev->data());
                receive_data(c);
                break;
            }
            case event::CONNECTION_WRITE_EVENT: {
				std::cout << "|!|IN_CONNECTION_WRITE_EVENT|!|" << std::endl;
                sock::Connection& c = static_cast<sock::Connection&>(*ev->data());
				std::vector<Config> resp_configs = getRespConfigs(c.request().headers(), configList_);
		
				if(c.request().requestLine().path().find("cgi-bin", 0) == true){
					std::cout << "|!|IN SCRIPT|!|" << std::endl;
					Script script(resp_configs[0], c.request());
					std::string ret =  script.exec();
					msg = ret.c_str();
					std::cout << "|!|OUT OF SCRIPT|!|" << std::endl;
				}
				else{
					struct stat info;
					stat(c.request().requestLine().path().c_str(), &info);
					//if (S_ISREG(info.st_mode) == true){
					std::cout << "|!|IN FILE RESPONSE|!|" << std::endl;
					Response response(c.request(), ConfigList_);
          			response.setHtmlBody();
          			response.setHtmlHeader();
					msg = response.full_content.c_str();
					std::cout << "|!|FILE RESPONSE BUILT|!|" << std::endl;
					}
					//else{
					//msg = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\n\r\n<h1>DEFAULT SERVER MESSAGE </h1>\r\n";
					//}
			std::cout << "|!|SENDING RESPONSE TO CLIENT|!|" << std::endl;
			send(c.fd(), msg, strlen(msg), 0);
			std::cout << "|!|RESPONSE SENT|!|" << std::endl;
			std::cout << "|!|CLOSING_CONNECTION|!|" << std::endl;
			close_connection(c);
			std::cout << "|!|CONNECTION_CLOSED|!|" << std::endl;
			break;
			}
        }
		std::cout << "|!|DELETING EVENT|!|" << std::endl;
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

std::vector<Config> getRespConfigs(http::HeaderMap header, std::vector<Config>& configList_){
  std::vector<Config> responseConfigs;
  http::HeaderMap::const_iterator it = header.get("host");
  std::string host = it->second;
  std::cout << "============This is host: " << host << "=============" << std::endl;
  for(unsigned long i = 0; i < configList_.size(); i++){
    if (host == configList_[i].listen.combined){
      responseConfigs.push_back(configList_[i]);
    }
  }
  return responseConfigs;
}
