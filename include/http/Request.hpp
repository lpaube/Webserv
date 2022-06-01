/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/30 16:19:56 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 20:33:04 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HeaderList.hpp"
#include "Method.hpp"
#include "QueryList.hpp"
#include <vector>

namespace http
{
class Request
{
public:
    Request(std::string request_str);

private:
    Method method_;
    std::string path_;
    QueryList query_;
    std::string http_version_;
    HeaderList headers_;
    std::string body_;
};

} // namespace http
