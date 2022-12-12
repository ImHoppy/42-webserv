#ifndef PARSING_HPP
# define PARSING_HPP

#include <exception>
#include <string>
#include <utility>
#include <vector>
#include <cstddef>
#include "GeneralConfig.hpp"
#include "Utils.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdint.h>

class GeneralConfig;

class ParsingError : public std::exception
{
	private:
	const char *	message;
	int				line;

	public:
	ParsingError();
	ParsingError(const char *msg);
	ParsingError(const char *msg, int lineNumber);
	virtual const char *	what() const throw();
	int				whatLine();
};

void	fillConfig(std::vector<std::pair<std::string, std::string> > key_value, GeneralConfig &);
void	parseConf(GeneralConfig &, std::string const & path );

#endif

