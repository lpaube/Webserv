/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Handlers.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/07 12:39:04 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/10 17:38:54 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Handlers.hpp"
#include "Buffer.hpp"
#include "Utils.hpp"
#include <exception>
#include <iostream>
#include <sstream>

namespace http
{

void parse_request_line(sock::Connection& c)
{
    RequestLine line;

    if (c.buffer().find(REQ_EOL, strlen(REQ_EOL)) == NULL) {
        // Bad request line
        return;
    }

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

    c.next_request_state();
    parse_headers(c);
}

void parse_headers(sock::Connection& c)
{
    Buffer& buf = c.buffer();
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

static void content_length_body(sock::Connection& c)
{
    http::Request& req = c.request();
    Buffer& buf = c.buffer();

    if (req.content_length() != 0) {
        size_t bytes =
            req.content_length() > buf.cursor_size() ? buf.cursor_size() : req.content_length();

        const char* start = buf.cursor();
        const char* end = start + bytes;
        req.body().append(start, end);
        req.read_body_bytes(bytes);
        buf.advance_cursor(bytes);
        buf.erase_to_cursor();

        if (req.content_length() == 0) {
            if (buf.cursor_size() != 0) {
                // Bad request
            }
            c.next_request_state();
            c.set_write();
        }
    }
}

static void chunked_request(sock::Connection& c)
{
    http::Request& req = c.request();
    Buffer& buf = c.buffer();

    while (true) {
        if (req.chunk_state() == http::C_CHUNK_SIZE) {
            size_t chunk_size = 0;
            // Read chunk size
            std::string line = get_next_word(buf, REQ_EOL, strlen(REQ_EOL));
            if (line.empty()) {
                break;
            }

            std::stringstream ss;
            ss << std::hex << line;
            ss >> chunk_size;
            if (ss.fail()) {
                // error
            }

            req.set_chunk_size(chunk_size);
            req.set_chunk_state(http::C_CHUNK);
        }

        // Done
        if (req.chunk_size() == 0) {
            if (buf.cursor_size() != 0) {
                // bad request
            }
            c.next_request_state();
            c.set_write();
            break;
        }

        if (req.chunk_state() == http::C_CHUNK) {
            // Can read the hole chunk
            if (req.chunk_size() + strlen(REQ_EOL) <= buf.cursor_size()) {
                req.body().append(buf.cursor(), buf.cursor() + req.chunk_size());
                buf.advance_cursor(req.chunk_size());
            } else {
                break;
            }

            if (buf.find(REQ_EOL, strlen(REQ_EOL)) != buf.cursor()) {
                // error
            }

            buf.advance_cursor(strlen(REQ_EOL));
            req.set_chunk_state(http::C_CHUNK_SIZE);
        }
    }

    buf.erase_to_cursor();
}

void parse_body(sock::Connection& c)
{
    http::Request& req = c.request();

    switch (req.body_type()) {
        case http::B_CONTENT_LENGTH:
            content_length_body(c);
            break;
        case http::B_CHUNKED:
            req.body().append(c.buffer().cursor(), c.buffer().cursor() + c.buffer().cursor_size());
            c.set_write();
            break;
        case http::B_NONE:
            chunked_request(c);
            c.next_request_state();
            break;
    }
}
} // namespace http
