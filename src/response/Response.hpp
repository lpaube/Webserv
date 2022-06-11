/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/08 15:56:58 by mafortin          #+#    #+#             */
/*   Updated: 2022/06/08 15:57:04 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#pragma once

class Response
{
  public:
    Response(/* args */) {}
    ~Response() {}
    std::string getHeader() const;
    void	setContentLength(std::size_t length);

  private:
    void  createCodeMsg();
    void  buildHeaderString();

  private:
    std::map<int, std::string> codeList;
    std::string contentLength;
    std::string location;
    std::string headerString;
    std::size_t	code;
    std::string codeMsg;
};
