#include <string>
#include <iostream>
namespace Logger
{
	void	Log(std::string const message, ...);
	void	Info(std::string const message, ...);
	void	Warning(std::string const message, ...);
	void	Error(std::string const message, ...);
}
