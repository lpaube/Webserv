/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   StatusCodes.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/16 16:45:48 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/16 17:04:14 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "StatusCodes.hpp"

/*
 * We can access the status code messages through a static method
 * like so: StatusCodes::getCodeMsg(code)
 */

#define INSERT_STATUS_CODE(code, name)                                                             \
    status_code_map.insert(std::pair<int, const char*>(code, name));

namespace StatusCode
{
std::map<int, const char*> status_code_map;
bool initialized = false;

void initialize_status_codes()
{
    INSERT_STATUS_CODE(100, "Continue");
    INSERT_STATUS_CODE(101, "Switching Protocols");
    INSERT_STATUS_CODE(102, "Processing");
    INSERT_STATUS_CODE(200, "Ok");
    INSERT_STATUS_CODE(201, "Created");
    INSERT_STATUS_CODE(202, "Accepted");
    INSERT_STATUS_CODE(203, "Non-Authoritative Information");
    INSERT_STATUS_CODE(204, "No Content");
    INSERT_STATUS_CODE(205, "Reset Content");
    INSERT_STATUS_CODE(206, "Partial Content");
    INSERT_STATUS_CODE(207, "Multi-Status");
    INSERT_STATUS_CODE(208, "Already Reported");
    INSERT_STATUS_CODE(226, "IM Used");
    INSERT_STATUS_CODE(300, "Multiple Choices");
    INSERT_STATUS_CODE(301, "Moved Permanently");
    INSERT_STATUS_CODE(302, "Found");
    INSERT_STATUS_CODE(303, "See Other");
    INSERT_STATUS_CODE(304, "Not Modified");
    INSERT_STATUS_CODE(305, "Use Proxy");
    INSERT_STATUS_CODE(307, "Temporary Redirect");
    INSERT_STATUS_CODE(308, "Permanent Redirect");
    INSERT_STATUS_CODE(400, "Bad Request");
    INSERT_STATUS_CODE(401, "Unauthorized");
    INSERT_STATUS_CODE(402, "Payment Required");
    INSERT_STATUS_CODE(403, "Forbidden");
    INSERT_STATUS_CODE(404, "Page not found");
    INSERT_STATUS_CODE(405, "Method Not Allowed");
    INSERT_STATUS_CODE(406, "Not Acceptable");
    INSERT_STATUS_CODE(407, "Proxy Authentification Required");
    INSERT_STATUS_CODE(408, "Request Timeout");
    INSERT_STATUS_CODE(409, "Conflict");
    INSERT_STATUS_CODE(410, "Gone");
    INSERT_STATUS_CODE(411, "Length Required");
    INSERT_STATUS_CODE(412, "Precondition Failed");
    INSERT_STATUS_CODE(413, "Request Entity Too Large");
    INSERT_STATUS_CODE(414, "Request URL Too Long");
    INSERT_STATUS_CODE(415, "Unsupported Media Type");
    INSERT_STATUS_CODE(416, "Requested Range not Satisfiable");
    INSERT_STATUS_CODE(417, "Expectation Failed");
    INSERT_STATUS_CODE(418, "I'm a Teapot");
    INSERT_STATUS_CODE(420, "Enhance Your Calm");
    INSERT_STATUS_CODE(422, "Unprocessable Entity");
    INSERT_STATUS_CODE(423, "Locked");
    INSERT_STATUS_CODE(424, "Failed Dependency");
    INSERT_STATUS_CODE(426, "Upgrade Required");
    INSERT_STATUS_CODE(428, "Precondition Required");
    INSERT_STATUS_CODE(429, "Too Many Request");
    INSERT_STATUS_CODE(431, "Request Header Fields Too Large");
    INSERT_STATUS_CODE(444, "No Response");
    INSERT_STATUS_CODE(449, "Retry With");
    INSERT_STATUS_CODE(450, "Blocked By Windows Parental Control");
    INSERT_STATUS_CODE(451, "Unavailable for Legal Reasons");
    INSERT_STATUS_CODE(499, "Client Closed Request");
    INSERT_STATUS_CODE(500, "Internal Server Error");
    INSERT_STATUS_CODE(501, "Not Implemented");
    INSERT_STATUS_CODE(502, "Bad Gateway");
    INSERT_STATUS_CODE(503, "Service Unavailable");
    INSERT_STATUS_CODE(504, "Gateway Timeout");
    INSERT_STATUS_CODE(505, "HTTP Version Not Supported");
    INSERT_STATUS_CODE(506, "Variant Also Negociates");
    INSERT_STATUS_CODE(507, "Insufficient Storage");
    INSERT_STATUS_CODE(508, "Loop Detected");
    INSERT_STATUS_CODE(509, "Bandwidth Limit Exceeded");
    INSERT_STATUS_CODE(510, "Not Extended");
    INSERT_STATUS_CODE(511, "Network Authentification Required");
    INSERT_STATUS_CODE(598, "Network Read Timeout Error");
    INSERT_STATUS_CODE(599, "Network Connect Timeout Error");
    initialized = true;
}

std::string get_code_msg(int code)
{
    if (!initialized) {
        initialize_status_codes();
    }

    std::map<int, const char*>::const_iterator it = status_code_map.find(code);

    return it == status_code_map.end() ? std::string() : std::string(it->second);
}
} // namespace StatusCode
