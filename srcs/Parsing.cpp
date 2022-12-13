
#include "Parsing.hpp"
#include <stack>
#include <cctype>

ParsingError::ParsingError() : message("Error on parsing."), line(-1) {}
ParsingError::ParsingError(const char *msg) : message(msg), line(-1) {}
ParsingError::ParsingError(const char *msg, int lineNumber) : message(msg), line(lineNumber) {}
const char *	ParsingError::what() const throw()
{
	return message;
}
int	ParsingError::whatLine()
{
	return line;
}
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
		if (!std::isdigit(*it))
			return false;
	return true;
}

const char *serverKeys[6] = {
	"host",
	"port",
	"server_names",
	"max_body_size",
	"root",
	"location"
};

void	check_key(std::string const & key, std::string const & parent, int lineNumber)
{
	if (parent == "head")
		throw ParsingError("Key not allowed in head", lineNumber);
	if (key.empty())
		throw ParsingError("Empty key", lineNumber);
	if (key.find_first_of(SpaceChar) != std::string::npos)
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
		throw ParsingError("The path location can only be in location object", lineNumber);
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

void	parseConf(GeneralConfig & config, std::string const & path )
{
	std::vector<std::pair<std::string, std::string> > key_value;
	std::ifstream	ifs;

	ifs.open(path.c_str());
	if (ifs.is_open() == false)
		throw ParsingError("File could not be open");
	std::string line;
	int32_t	lineNumber = 0;
	int	depth = 0;
	std::stack<std::string> parents;
	parents.push("head");
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
		if (value == "{")
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
			checkParentObject(parents.top(), key, lineNumber);
			parents.push(key);
		}
		else if (key == "}")
		{
			if (parents.size() > 1)
				parents.pop();
		}
		else
			check_key(key, parents.top(), lineNumber);
		key_value.push_back(std::make_pair(key, value));
	}
	if (depth != 0 || parents.top() != "head")
		throw ParsingError("Bracket not closed");
	ifs.close();
	fillConfig(key_value, config);
}
