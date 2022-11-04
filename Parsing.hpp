#pragma once

#include <exception>
#include <string>
#include <utility>
#include <vector>

class ParsingError : public std::exception
{
	private:
	const char *	message;
	int				line;

	public:
	ParsingError();
	ParsingError(const char *msg);
	ParsingError(const char *msg, int lineNumber);
	const char *	what();
	int				whatLine();
};

void	fillConfig(std::vector<std::pair<std::string, std::string> > key_value);