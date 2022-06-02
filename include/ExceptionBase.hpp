/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionBase.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 22:00:37 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:03:30 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <exception>

class ExceptionBase : public std::exception
{
public:
    static const std::size_t MSG_SIZE = 512;

public:
    ExceptionBase(const char* msg);

public:
    virtual const char* what() const throw();

private:
    char msg_[MSG_SIZE];
};
