#include <string>
#include <fstream>
#include <iostream>
#include "GeneralConfig.hpp"
#include <sstream>
#include "Parsing.hpp"

ParsingError::ParsingError() : message("Error on parsing."), line(-1) {}
ParsingError::ParsingError(const char *msg) : message(msg), line(-1) {}
ParsingError::ParsingError(const char *msg, int lineNumber) : message(msg), line(lineNumber) {}
const char *	ParsingError::what()
{
	return message;
}
int	ParsingError::whatLine()
{
	return line;
}

const char* ws = " \t\n\r\f\v";

// trim from end of string (right)
inline std::string& rtrim(std::string& s, const char* t = ws)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = ws)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
inline std::string& trim(std::string& s, const char* t = ws)
{
    return ltrim(rtrim(s, t), t);
}

std::string	IntToStr(int32_t	i)
{
	std::ostringstream s;
	s << i;
	return s.str();
}

void	parseConf(std::string const & path )
{
	std::ifstream	ifs;

	ifs.open(path);
	if (ifs.is_open() == false)
	{
		std::cerr << "File could not be open." << std::endl;
		return ;
	}
	std::string line;
	int32_t	lineNumber = 0;
	int	deep = 0;
	while (std::getline(ifs, line))
	{
		lineNumber++;
		line = trim(line);

		std::string	key;
		std::string	value;
		{
			std::istringstream	stream_line(line);
			std::getline(stream_line, line, '#');
		}
		std::istringstream	stream_line(line);
		if (std::getline(stream_line, key, '='))
		{
			if (std::getline(stream_line, value))
			{
				if (trim(value) == "}")
					--deep;
			}
			else
			{
				if (key != "{" && key != "}")
					throw ParsingError("cant have nothing after key =", lineNumber);
			}
			if (trim(key) == "}")
				--deep;
		}
		key = trim(key);
		value = trim(value);
		if (key.empty() && value.empty())
			continue;
		if (deep < 0)
			throw ParsingError("Cant close not open bracket");
		std::cout << std::string(deep, '\t') << key;
		if (!value.empty())
			std::cout << " | " << value;
		std::cout << "\n";
		if (value == "{")
			++deep;
		if (key == "{")
			++deep;

		// Check error
		if (key != "}" && value.empty())
			throw ParsingError( "Key is empty",lineNumber);
		if (key.find('{') != std::string::npos || (key.length() > 2 && key.find('{') != std::string::npos))
			throw ParsingError( "{ or } can't contains in a key", lineNumber);
		if (!value.empty() && key == "}")
			throw ParsingError( "} can't have value", lineNumber);
		
		
	}
	if (deep != 0)
		throw ParsingError("Bracket not closed");
	ifs.close();

}