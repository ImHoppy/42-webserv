#include "Utils.hpp"

inline std::string& rtrim(std::string& s, const char* t = SpaceChar)
{
    s.erase(s.find_last_not_of(t) + 1);
    return s;
}

// trim from beginning of string (left)
inline std::string& ltrim(std::string& s, const char* t = SpaceChar)
{
    s.erase(0, s.find_first_not_of(t));
    return s;
}

// trim from both ends of string (right then left)
std::string& trim(std::string& s, const char* t)
{
    return ltrim(rtrim(s, t), t);
}

std::string & trim_quote(std::string & s, int lineNumber)
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
		if (std::count(s.begin(), s.end(), typeQuote) > 0)
			throw ParsingError("Quoted string cant contains self quote", lineNumber);
	}
	else if ((s.at(0) == '"' || s[s.size()-1] == '"') \
	|| (s.at(0) == '\'' || s[s.size()-1] == '\''))
		throw ParsingError("Unfinished quote", lineNumber);
	return (s);
}
