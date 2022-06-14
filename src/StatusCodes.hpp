#pragma once

#include <map>
#include <iostream>
#include <string>

struct status_codes {
  static std::map<int, std::string> status_code_map;
  static bool initialized;

  static std::string getCodeMsg(int code);

  static void initialize_status_codes();
};
