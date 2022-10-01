#pragma once

#include <map>
#include <string>

/*
 * We can access the status code messages through a static method
 * like so: StatusCodes::getCodeMsg(code)
 * See StatusCodes.cpp for all the status codes
 */

namespace StatusCode
{
static std::map<int, const char*> codes;

std::string get_code_msg(int code);
} // namespace StatusCode
