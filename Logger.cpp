#include "Logger.hpp"
#include <sstream>
#include <iostream>
#include <ctime>
#include <stdio.h>
#include <stdarg.h>
#include <iomanip>

#define BLUE "\033[0;34m"
#define GREEN "\033[0;32m"

#define LogFunc(stdout, fd, color, type) \
{ \
	va_list args; \
	va_start(args, message); \
	dprintf(fd, "%s %s[%s] ", stringTimestamp().c_str(), color, type); \
	vdprintf(fd, message.c_str(), args); \
	stdout << "\033[39m" << std::endl; \
	va_end(args); \
}

namespace Logger
{
	static std::string stringTimestamp()
	{
		std::time_t time = std::time(0);
		std::tm *tm = std::localtime(&time);
		std::stringstream ss;

		ss << std::setfill('0') << "\033[90m["

		<< std::setw(2) << tm->tm_hour << ":"
		<< std::setw(2) <<tm->tm_min << ":"
		<< std::setw(2) << tm->tm_sec
		<< ' '
		<< std::setw(2) << tm->tm_mday << "/"
		<< std::setw(2) << (tm->tm_mon + 1) << "/"
		<< std::setw(4) << (tm->tm_year + 1900)
		<< "]\033[39m";
		return ss.str();
	}
	
	void	Log(std::string const message, ...)
	{
		va_list args;
		va_start(args, message);
		stringTimestamp();
		vprintf(message.c_str(), args);
		va_end(args);
	}

	void	Info(std::string const message, ...)
	LogFunc(std::cout, 1, GREEN, "INFO")

	void	Warning(std::string const message, ...)
	LogFunc(std::cerr, 2, "\033[93m", "WARNING")

	void	Error(std::string const message, ...)
	LogFunc(std::cerr, 2, "\033[91m", "ERROR")
}
