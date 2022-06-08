/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 12:39:04 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/07 14:55:45 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"
#include "Utils.hpp"
#include "sock/Buffer.hpp"
#include <iostream>

namespace http
{

void parse_request_line(sock::Connection& c)
{
    RequestLine line;
    bool error = false;

    if (c.buffer().find(REQ_EOL, strlen(REQ_EOL)) == NULL) {
        return;
    }

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

    c.next_request_state();
    parse_headers(c);
}

void parse_http_request_line(sock::Connection& c)
{
    RequestLine line;
    bool error = false;

    if (c.buffer().find(REQ_EOL, strlen(REQ_EOL)) == NULL) {
        return;
    }

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

    c.next_request_state();
    parse_headers(c);
}

void parse_headers(sock::Connection& c)
{
    sock::Buffer& buf = c.buffer();
    const char* ptr;
    while ((ptr = buf.find(REQ_EOL, strlen(REQ_EOL))) != NULL) {
        if (ptr == buf.cursor()) {
            c.next_request_state();
            buf.advance_cursor(strlen(REQ_EOL));
            break;
        }

        try {
            http::Header header(get_next_word(buf, REQ_EOL, strlen(REQ_EOL)));
            c.request().add_header(header);
        } catch (const std::exception& ex) {
            std::cout << ex.what() << std::endl;
        }
    }
    buf.erase_to_cursor();

    if (c.request_state() == http::REQ_BODY) {
        parse_body(c);
    }
}

void parse_body(sock::Connection& c)
{
    c.set_write();
}
} // namespace http
