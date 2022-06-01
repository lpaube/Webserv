/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 20:36:33 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 22:13:16 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>
#include "LocationContent.hpp"
class ServerParser{
	public:
		ServerParser(std::string::iterator beg, std::string::iterator end);
		~ServerParser(){};
	private:
		std::vector<std::string> server_content;
		std::vector<LocationContent>	location;
};
