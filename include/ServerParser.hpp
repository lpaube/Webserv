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
		void	printContent() const;
		void	printLocation() const;
		class NoSepException : public std::exception{
			public:
				virtual const char* what() const throw();
		};
		class SyntaxException : public std::exception{
			public:
				virtual const char* what() const throw();
		};
	private:
		void		buildContent();
		void		buildLocation();
		std::size_t	findLocStart(std::size_t i);
		std::size_t	findLocEnd(std::size_t i, std::size_t end);

		std::vector<std::string> server_content;
		std::vector<LocationContent>	location;
		std::string	str_content;
		int								nb_location;
};
