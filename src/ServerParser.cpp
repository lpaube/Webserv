#include "../include/ServerParser.hpp"
#include <iostream>

ServerParser::ServerParser(std::string::iterator beg, std::string::iterator end) : str_content(beg, end){
}

void	ServerParser::buildContent(){
	std::string::iterator start = str_content.begin();
	std::string::iterator end = str_content.end();
	std::string::iterator current = start;
	while (current != end){
		if (*current == ';'){
			std::string add(start, current - 1);
			server_content.push_back(add);
			start = current + 1;
			current = start;
		}
		else if (*current == "l")
		{
			
		}
		else
			current++;

	}
}