#pragma once
#include <string>
#include <stdint.h>

#define SpaceChar " \t\n\r\f\v"

int32_t		StrToInt(std::string const & str);
int32_t		StrToIp(std::string const & ip);
bool		isIpv4(std::string const & s);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::string &	trim_quote(std::string & s, int lineNumber = -1);
std::string &	trim(std::string& s, const char* t = SpaceChar);
