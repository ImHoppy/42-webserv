#ifndef LOGGER_HPP
# define LOGGER_HPP

# include <string>
# include <iostream>
# include <iomanip>
# include <ctime>
# include <stdio.h>
# include <stdarg.h>

namespace Logger
{
	void	Log(std::string const message, ...);
	void	Info(std::string const message, ...);
	void	Warning(std::string const message, ...);
	void	Error(std::string const message, ...);
}

#endif

