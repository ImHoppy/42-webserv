#ifndef UTILS_HPP
# define UTILS_HPP

#include <string>
#include <vector>
#include <sstream>
#include <stdint.h>
// OpenDir
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
// Trim.cpp
#include <string>
#include <algorithm>
#include "Parsing.hpp"

#define SpaceChar " \t\n\r\f\v"

char	generateChar(void);
std::string	generateFileName(int seed);

bool	ends_with(const std::string& str, const std::string& extension);
bool	ends_with(const std::string& str, const char c);
bool	startsWith(std::string const & str, std::string const & start);
std::string::iterator	findCRLF(std::string::iterator start, std::string::iterator end);
std::vector<std::string>	listFiles(std::string const & path, bool withDot = true);
std::string		getFileExtension(const std::string & filename);
std::string	generateErrorBody(std::pair<int, std::string> code);
std::string	GenerateHtmlDirectory(std::string const & path);
std::string	IntToStr(int nb);
int32_t		StrToInt(std::string const & str);
int32_t		StrToIp(std::string const & ip);
bool		isIpv4(std::string const & s);
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

std::string &	trim_quote(std::string & s, int lineNumber = -1);
std::string &	trim(std::string& s, const char* t = SpaceChar);

std::string getExtension(std::string const & filename);

#endif

