#include "GeneralConfig.hpp"
#include "LocationConfig.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include "Parsing.hpp"
#include "Utils.hpp"

// string ip to int32_t

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
//				if (errfile.is_open() == false)
//					return false;
				server.addErrorPaths(StrToInt(it->first), it->second);
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
	if (server.getMaxBodySize() < 1024)// TODO: Need to check if 8e+6 is a valid size and accept 1G 1M 1K ? 
		throw ParsingError("Server must have a max body size");
	config.addServer(server);
}

#include <algorithm>
bool	ServerNamesIsEqual(std::vector<std::string> const & a, std::vector<std::string> const & b)
{
	std::vector<std::string>::const_iterator it = a.begin();
	while (it != a.end())
	{
		if (std::find(b.begin(), b.end(), *it) != b.end())
			return true;
		++it;
	}
	return false;
}

void	CheckVirtualServer(GeneralConfig & config)
{
	std::vector<ServerConfig> servers = config.getServers();
	std::vector<ServerConfig>::iterator it = servers.begin();
	std::vector<ServerConfig>::iterator it2 = servers.begin();
	while (it != servers.end())
	{
		it2 = it + 1;
		while (it2 != servers.end())
		{
			if (it->getPort() == it2->getPort())
			{
				if (ServerNamesIsEqual(it->getServerNames(), it2->getServerNames()))
					throw ParsingError("Duplicate virtual server");
				if (it->getServerNames().empty() || it2->getServerNames().empty())
					throw ParsingError("Duplicate port without server names");
				if (it->getHost() != it2->getHost())
					throw ParsingError("Host must be the same for virtual servers");
			}
			++it2;
		}
		++it;
	}
}

void	fillConfig(key_value_t key_value, GeneralConfig & config)
{
	std::vector<std::pair<std::string, std::string> >::iterator it = key_value.begin();
	while (it != key_value.end())
	{
		if (it->first == "server")
		{
			serverBlock(config, ++it);
		}
		++it;
	}
	key_value.clear();
	CheckVirtualServer(config);
	// config.printConfig();
}
