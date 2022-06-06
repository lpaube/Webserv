/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ExceptionBase.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mleblanc <mleblanc@student.42quebec.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 22:02:26 by mleblanc          #+#    #+#             */
/*   Updated: 2022/05/31 22:05:45 by mleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ExceptionBase.hpp"
#include <cstring>

ExceptionBase::ExceptionBase(const char* msg)
{
    strncpy(msg_, msg, MSG_SIZE);
    msg_[MSG_SIZE - 1] = 0;
}

const char* ExceptionBase::what() const throw()
{
    return msg_;
}
