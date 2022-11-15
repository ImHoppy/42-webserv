#pragma once
#include <string>
#include <stdint.h>
#include <vector>

#define SpaceChar " \t\n\r\f\v"

std::string	generateErrorBody(std::string title, std::string body);
std::string	generateErrorBody(std::string body);
std::string generateResponse(std::string fileContent);
std::string generateResponse(int code, std::string codeMsg, std::string fileContent);
std::string	GenerateHtmlDirectory(std::string const & path);
bool	endsWithSlash(const std::string & path);
std::string	nbToString(int nb);
int32_t		StrToInt(std::string const & str);
int32_t		StrToIp(std::string const & ip);
bool		isIpv4(std::string const & s);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::string &	trim_quote(std::string & s, int lineNumber = -1);
std::string &	trim(std::string& s, const char* t = SpaceChar);

std::string getExtension(std::string const & filename);
