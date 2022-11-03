#pragma once

#include <exception>
#include <string>

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