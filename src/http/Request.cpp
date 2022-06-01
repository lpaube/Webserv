/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:52:09 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:40:59 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "http/Request.hpp"
#include "http/Method.hpp"
#include "http/Query.hpp"
#include "Utils.hpp"
#include <stdexcept>

namespace http
{
Request::Request(std::string request_str)
{
    std::string request_line = get_next_word(request_str, "\r\n");
    std::string str = get_next_word(request_line, " ");
    method_ = method_from_str(str);
    if (method_ == BAD_METHOD) {
        throw std::runtime_error("Bad http method");
    }
    str = get_next_word(request_line, "?");
    if (!str.empty()) {
        path_ = str;
        query_ = QueryList(get_next_word(request_line, " "));
    } else {
        path_ = get_next_word(request_line, " ");
    }
    http_version_ = request_line;

    while (request_str.find("\r\n") != 0) {
        headers_.add(Header(get_next_word(request_str, "\r\n")));
    }

    body_ = request_str;
}

} // namespace http
