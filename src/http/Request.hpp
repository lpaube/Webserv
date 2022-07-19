/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/13 17:17:57 by mleblanc          #+#    #+#             */
/*   Updated: 2022/07/19 14:04:02 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "core/ExceptionBase.hpp"
#include <map>
#include <string>
#include <unistd.h>
#include <vector>

enum ParseState {
    REQ_METHOD,
    REQ_PATH,
    REQ_QUERY,
    REQ_HTTP_VERSION,
    REQ_LINE_ALMOST_DONE,
    REQ_HEADER_START,
    REQ_HEADER_NAME,
    REQ_HEADER_VALUE,
    REQ_HEADER_ALMOST_DONE,
    REQ_ALL_HEADERS_ALMOST_DONE,
    REQ_BODY_START,
    REQ_BODY
};

enum Method {
    BAD_METHOD,
    GET,
    POST,
    DELETE,
    OPTIONS
};

enum ChunkState {
    CNK_SIZE,
    CNK_CHUNK,
    CNK_NL
};

class Request
{
    friend class TcpConnection;

public:
    typedef std::multimap<std::string, std::string>::const_iterator header_iterator;

public:
    class Exception : public ExceptionBase
    {
    public:
        Exception(const std::string& msg, int code);

        int status_code() const;

    private:
        int status_code_;
    };

public:
    Request();

    Method method() const;
    const std::string& path() const;
    const std::string& query_str() const;
    const std::string& http_version() const;
    const std::vector<char>& body() const;
    header_iterator find_header(const std::string& name) const;
    header_iterator headers_end() const;
    void print() const;

private:
    typedef std::vector<char>::const_iterator const_rbody_iter;
    typedef std::vector<char>::iterator rbody_iter;

private:
    bool chunked() const;
    ssize_t cur_chunk_size() const;
    bool content_length() const;
    size_t content_length_count() const;
    void set_state_and_clear_buf(ParseState state);
    // Also clears the request buffer
    void set_state_and_value(ParseState state, void (Request::*setter)(const std::string&));
    ParseState parse_state() const;
    void set_parse_state(ParseState state);
    void set_method(Method method);
    void set_path(const std::string& path);
    void set_query_str(const std::string& query);
    void set_http_version(const std::string& version);
    const std::string& cur_header() const;
    void set_cur_header(const std::string& name);
    const std::string& buffer() const;
    void clear_buf();
    void append_buf(char c);
    void add_header(const std::string& name, const std::string& value);
    void process_headers();
    void append_raw_body(const std::vector<char>& data);
    void decode_raw_body();
    bool all_chunks_received() const;
    bool read_chunk_size();
    bool read_chunk();

private:
    ParseState state_;
    Method method_;
    std::string path_;
    std::string query_str_;
    std::string http_version_;
    std::multimap<std::string, std::string> headers_;
    std::string buffer_;
    std::string cur_header_name_;
    bool is_content_length;
    size_t content_length_count_;
    bool is_chunked;
    ssize_t cur_chunk_size_;
    ChunkState cnk_state_;
    std::vector<char> raw_body_;
    std::vector<char> body_;
};

const char* method_str(Method method);
Method method_from_str(const std::string& str);
