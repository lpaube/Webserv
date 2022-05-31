/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   configparser.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/05/31 19:07:03 by mafortin          #+#    #+#             */
/*   Updated: 2022/05/31 19:18:31 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include <string>

class ConfigParser
{
public:
	ConfigParser(std::string config_file);
	~ConfigParser();
	class ConfigFileException : public std::exception{
		public:
			virtual const char* what() const throw();
	};
private:
};
