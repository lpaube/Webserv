#pragma once

#include <map>
#include <iostream>
#include <string>

/*
 * We can access the status code messages through a static method
 * like so: StatusCodes::getCodeMsg(code)
 * See StatusCodes.cpp for all the status codes
 */

struct StatusCodes {
  static std::map<int, std::string> status_code_map;
  static bool initialized;

  static std::string getCodeMsg(int code);

  static void initialize_status_codes();
};
