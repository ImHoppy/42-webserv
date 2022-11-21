#include "Utils.hpp"

std::string		getFileExtension(const std::string & filename)
{
	std::string		extension;
	size_t	posDot = filename.find_last_of('.', filename.find_last_of('/'));
	if (posDot == std::string::npos)
	{
		extension = "plain";
		return extension;
	}
	extension = filename.substr(posDot + 1);
	return extension;
}

bool	ends_with(const std::string& str, const char c)
{
	if (str.empty())
		return false;
	if (*(str.end() - 1) != c)
		return false;
	return true;
}

bool	ends_with(const std::string& str, const std::string& extension)
{
	std::string::size_type	pos;
	pos = str.rfind(extension);
	if (pos != std::string::npos && pos == str.size() - extension.size())
		return true;
	return false;
}

std::string	nbToString(int nb)
{
	std::stringstream	ss;
	ss << nb;
	return ss.str();
}

int32_t		StrToInt(std::string const & str)
{
	std::istringstream s(str);
	int32_t i;
	s >> i;
	return i;
}

int32_t		StrToIp(std::string const & ip)
{
	std::stringstream s(ip);
	int a,b,c,d;
	char ch;
	s >> a >> ch >> b >> ch >> c >> ch >> d;
	return (a << 24) + (b << 16) + (c << 8) + d;
}

bool	isIpv4(std::string const & s)
{
	int		count = 0;
	int		i = 0;
	int		nb = 0;
	std::string::const_iterator it;

	for (it = s.begin(); it < s.end(); it++)
	{
		if (*it == '.')
		{
			if (nb > 255)
				return false;
			nb = 0;
			count++;
			i = 0;
		}
		else if (*it >= '0' && *it <= '9')
		{
			nb = nb * 10 + (*it - '0');
			i++;
		}
		else
			return false;
	}
	if (count != 3)
		return false;
	return true;
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::string getExtension(std::string const & filename)
{
	std::string::size_type pos = filename.find_last_of('.');
	if (pos == std::string::npos)
		return "";
	return filename.substr(pos + 1);
}

/*
Find in the range [first, last) the first occurence of CRLF ("\r\n"). Return an iterator
to '\r' if found, end otherwise.
*/
std::string::iterator	findCRLF(std::string::iterator start, std::string::iterator end)
{
	while (start != end)
	{
		if (*start == '\r' && (start + 1) != end && *(start + 1) == '\n')
			return start;
		++start;
	}
	return end;
}
