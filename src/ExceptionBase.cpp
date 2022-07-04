/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionBase.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 22:02:26 by mleblanc          #+#    #+#             */
/*   Updated: 2022/06/08 19:23:00 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExceptionBase.hpp"
#include <cstring>

ExceptionBase::ExceptionBase(const std::string& msg)
{
    strlcpy(msg_, msg.c_str(), MSG_SIZE);
}

const char* ExceptionBase::what() const throw()
{
    return msg_;
}
