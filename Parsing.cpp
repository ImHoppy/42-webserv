#include <string>
#include <fstream>
#include <iostream>
#include "GeneralConfig.hpp"
#include <sstream>
#include <algorithm>
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



std::string & trim_quote(std::string & s, int lineNumber = -1)
{
	trim(s);
	if (s.empty())
		return (s);
	char typeQuote = 0;
	if ((s.at(0) == '"' && s[s.size()-1] == '"') \
	|| (s.at(0) == '\'' && s[s.size()-1] == '\''))
	{
		typeQuote = (s.at(0) == '"') ? '"' : '\'';
		s = s.substr(1, s.size() - 2);
		if (count_char(s, typeQuote) > 0)
			throw ParsingError("Quoted string cant contains self quote", lineNumber);
	}
	else if ((s.at(0) == '"' || s[s.size()-1] == '"') \
	|| (s.at(0) == '\'' || s[s.size()-1] == '\''))
		throw ParsingError("Unfinished quote", lineNumber);
	return (s);
}

static int	count_char(std::string const & s, char c)
{
	int	count = 0;
	std::string::const_iterator it;

	for (it = s.begin(); it < s.end(); it++)
		if (*it == c)
			++count;
	return count;
}

std::string	IntToStr(int32_t	i)
{
	std::ostringstream s;
	s << i;
	return s.str();
}

// const std::map<std::string, std::string> {{"server", "host"}};

// Check if std::string is in array
bool	is_in_array(std::string const & s, char const ** array, int size)
{
	for (int i = 0; i < size; i++)
		if (s == array[i])
			return true;
	return false;
}

// Check if std::string is only composed of digits
bool	is_only_digits(std::string const & s)
{
	for (std::string::const_iterator it = s.begin(); it < s.end(); it++)
		if (!isdigit(*it))
			return false;
	return true;
}

const char *serverKeys[7] = {
	"host",
	"port",
	"server_name",
	"max_body_size",
	"root",
	"autoindex",
	"location"
};

void	check_key(std::string const & key, std::string const & parent, int lineNumber)
{
	if (parent == "head")
		throw ParsingError("Key not allowed in head", lineNumber);
	if (key.empty())
		throw ParsingError("Empty key", lineNumber);
	if (key.find_first_of(ws) != std::string::npos)
		throw ParsingError("Key cant contains whitespace", lineNumber);
	if (key.find_first_of("[]") != std::string::npos)
		throw ParsingError("Key cant contains []", lineNumber);

	if (parent == "server")
	{
		if (!is_in_array(key, serverKeys, sizeof(serverKeys)/sizeof(char*)))
			throw ParsingError("Unknown key in server Object", lineNumber);
	}
	if (parent == "error" && !is_only_digits(key))
		throw ParsingError("Error code must be a number", lineNumber);
}

void	checkParentObject(std::string const & parent, std::string const & key, int lineNumber = -1)
{
	if (key == parent)
		throw ParsingError("an object cannot be in an object with the same name", lineNumber);
	if (key == "server" && parent != "head")
		throw ParsingError("server cannot be in another object", lineNumber);
	else if (parent != "server" && key == "error")
		throw ParsingError("error can only be in server object", lineNumber);
	else if (parent != "server" && key == "location")
		throw ParsingError("location can only be in server object", lineNumber);
	if (key != "location" && key != "server" && key != "error" && parent != "location")
		throw ParsingError("Cant have a location in a another location", lineNumber);
	// Usuless ?
	// if (parent != "location")
	// {
	// 	if (key != "server" && key != "error" && key != "location")
	// 		throw ParsingError("Seulement location object", lineNumber);
	// }
	if (parent == "server" && (key != "location" && key != "error"))
		throw ParsingError("Unknow key in object Server", lineNumber);
}

// Remove comment from line
static void	remove_comment(std::string & s)
{
	size_t pos = s.find('#');
	if (pos != std::string::npos)
		s = s.substr(0, pos);
}

void	parseConf(std::string const & path )
{
	std::ifstream	ifs;

	ifs.open(path.c_str());
	if (ifs.is_open() == false)
	{
		std::cerr << "File could not be open." << std::endl;
		return ;
	}
	std::string line;
	int32_t	lineNumber = 0;
	int	depth = 0;
	std::vector<std::string> parents;
	parents.push_back("head");
	while (std::getline(ifs, line))
	{
		lineNumber++;
		remove_comment(line);
		line = trim(line);
		if (line.empty())
			continue;

		std::string	key;
		std::string	value;
		std::istringstream	stream_line(line);
		if (std::getline(stream_line, key, '='))
		{
			std::getline(stream_line, value);
			if (value.empty() && key != "{" && key != "}")
				throw ParsingError("cant have nothing after key =", lineNumber);
			if (trim(key) == "}")
				--depth;
		}
		key = trim(key);
		value = trim_quote(value, lineNumber);
		if (key.empty() && value.empty())
			continue;
		if (depth < 0)
			throw ParsingError("Cant close not open bracket");
		std::cout << std::string(depth, '\t') << key;
		if (!value.empty())
			std::cout << " | " << value;
		std::cout << "\n";
		if (value == "{")
			++depth;
		if (key == "{")
			++depth;

		// Check error
		if (key != "}" && value.empty())
			throw ParsingError( "Key is empty",lineNumber);
		if (key.find('{') != std::string::npos || (key.length() > 2 && key.find('{') != std::string::npos))
			throw ParsingError( "{ or } can't contains in a key", lineNumber);
		if (!value.empty() && key == "}")
			throw ParsingError( "} can't have value", lineNumber);
		if (value == "{")
		{
			checkParentObject(parents.back(), key, lineNumber);
			parents.push_back(key);
		}
		else if (key == "}")
		{
			if (parents.size() > 1)
				parents.pop_back();
		}
		else
			check_key(key, parents.back(), lineNumber);
	}
	if (depth != 0)
		throw ParsingError("Bracket not closed");
	ifs.close();

}