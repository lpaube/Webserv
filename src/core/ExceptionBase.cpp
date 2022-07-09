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
#include <algorithm>

ExceptionBase::ExceptionBase(const std::string& msg)
{
    const std::size_t len = msg.size() > MSG_SIZE - 1 ? MSG_SIZE - 1 : msg.size();
    std::copy(msg.begin(), msg.begin() + (long)len, msg_);
    msg_[len] = 0;
}

const char* ExceptionBase::what() const throw()
{
    return msg_;
}
