#include "GeneralConfig.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include "Parsing.hpp"
#include "Utils.hpp"

// string ip to int32_t
static int32_t		StrToIp(std::string const & ip)
{
	std::stringstream s(ip);
	int a,b,c,d;
	char ch;
	s >> a >> ch >> b >> ch >> c >> ch >> d;
	return (a << 24) + (b << 16) + (c << 8) + d;
}

static bool	isIpv4(std::string const & s)
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

// TODO(mbraets): Create utils files or class
// is string is only digit
static bool	isDigit(std::string const & s)
{
	std::string::const_iterator it;

	for (it = s.begin(); it < s.end(); it++)
	{
		if (*it < '0' || *it > '9')
			return false;
	}
	return true;
}

typedef std::vector<std::pair<std::string, std::string> > key_value_t;

void	locationBlock(ServerConfig & server, key_value_t::iterator & it)
{
	while (it->first != "}")
	{
		LocationConfig location(it->first);
		++it;
		while (it->first != "}")
		{
			if (it->first == "root")
				location.setRootPath(it->second);
			else if (it->first == "index")
				location.setIndexFile(it->second);
			else if (it->first == "dir_list")
			{
				if (it->second == "true")
					location.setDirList(true);
				else if (it->second == "false")
					location.setDirList(false);
				else
					throw ParsingError("dir_list must be a boolean");
			}
			else if (it->first == "redirect")
				location.setRedirUrl(it->second);
			else if (it->first == "cgi_cmd")
				location.setCGICmd(it->second);
			else if (it->first == "method")
			{
				// split space separated methods and check if they are valid
				std::vector<std::string> methods;
				split(it->second, ' ', methods);
				std::vector<std::string>::iterator it2 = methods.begin();
				while (it2 != methods.end())
				{
					if (*it2 == "GET")
						location.addMethod(LocationConfig::GET);
					else if (*it2 == "POST")
						location.addMethod(LocationConfig::POST);
					else if (*it2 == "DELETE")
						location.addMethod(LocationConfig::DELETE);
					else
						throw ParsingError("Invalid method");
					++it2;
				}
			}
			else {
				std::cout << "Unknown key: " << it->first << std::endl;
				throw ParsingError("Invalid Location key");
			}
			++it;
		}
		if (location.isRedirection() && !location.isEmpty())
			throw ParsingError("Location can't be a redirection and have a file or a cgi");
		if (not location.isRedirection() && location.getMethods().none())
			location.addMethod(LocationConfig::GET);
		if (location.getIndexFile().empty())
			location.setIndexFile("index.html");
		if (location.getRootPath().empty())
			location.setRootPath(server.getRootPath());
		server.addLocation(location.getPath(), location);
		++it;
	}
}

void	serverBlock(GeneralConfig & config, key_value_t::iterator & it)
{
	ServerConfig server;
	while (it->first != "}")
	{
		if (it->first == "host")
		{
			if (isIpv4(it->second))
				server.setHost(StrToIp(it->second));
			else
				throw ParsingError("Host must be an IPv4 address");
		}
		else if (it->first == "port")
		{
			if (isDigit(it->second))
				server.setPort(StrToInt(it->second));
			else
				throw ParsingError("Port must be a number");
		}
		else if (it->first == "server_names")
		{
			std::vector<std::string> server_names;
			split(it->second, ' ', server_names);
			server.setServerNames(server_names);
		}
		else if (it->first == "error")
		{
			++it;
			while (it->first != "}")
			{
				server.addErrorPage(StrToInt(it->first), it->second);
				++it;
			}
		}
		else if (it->first == "max_body_size")
		{
			if (isDigit(it->second))
				server.setMaxBodySize(StrToInt(it->second));
			else
				throw ParsingError("Max body size must be a number");
		}
		else if (it->first == "root")
		{
			server.setRootPath(it->second);
		}
		else if (it->first == "location")
		{
			locationBlock(server, ++it);
		}
		else {
			std::cout << it->first << std::endl;
			throw ParsingError("Invalid server key");
		}
		++it;
	}
	if (server.getPort() == 0)
		throw ParsingError("Server must have a port");
	if (server.getRootPath().empty())
		throw ParsingError("Server must have a root");
	config.addServer(server);
}

void	fillConfig(key_value_t key_value)
{
	std::vector<std::pair<std::string, std::string> >::iterator it = key_value.begin();
	GeneralConfig config;
	while (it != key_value.end())
	{
		if (it->first == "server")
		{
			serverBlock(config, ++it);
		}
		++it;
	}
	key_value.clear();
	config.printConfig();
}
