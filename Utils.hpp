#pragma once
#include <string>
#include <stdint.h>

#define SpaceChar " \t\n\r\f\v"

int32_t StrToInt(std::string const & str);
std::string &	trim_quote(std::string & s, int lineNumber = -1);
std::string &	trim(std::string& s, const char* t = SpaceChar);
