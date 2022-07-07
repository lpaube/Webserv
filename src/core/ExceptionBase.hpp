/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionBase.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 22:00:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 19:22:38 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>
#include <string>

class ExceptionBase : public std::exception
{
public:
    static const std::size_t MSG_SIZE = 512;

public:
    ExceptionBase(const std::string& msg);

    virtual const char* what() const throw();

private:
    char msg_[MSG_SIZE];
};
