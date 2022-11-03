#pragma once

#include <string>
#include <vector>
#include "ServerConfig.hpp"
#include <iostream>

class GeneralConfig {
	private:
	std::vector<ServerConfig>	_servers;

	public:
	GeneralConfig();
	GeneralConfig(const GeneralConfig &other);
	GeneralConfig &operator=(const GeneralConfig &other);
	~GeneralConfig();

	void	addServer(const ServerConfig &server);
	void printConfig() {
		for (std::vector<ServerConfig>::iterator it = _servers.begin(); it != _servers.end(); it++) {
			std::cout << "Host: " << it->getHost() << std::endl;
			std::cout << "Port: " << it->getPort() << std::endl;
			std::cout << "MaxBodySize: " << it->getMaxBodySize() << std::endl;
			std::cout << "ErrorPages: " << std::endl;
			for (std::map<int, std::string>::const_iterator it2 = it->getErrorPages().begin(); it2 != it->getErrorPages().end(); it2++) {
				std::cout << it2->first << " " << it2->second << std::endl;
			}
			std::cout << "Locations: " << std::endl;
			for (std::map<std::string, LocationConfig>::const_iterator it2 = it->getLocations().begin(); it2 != it->getLocations().end(); it2++) {
				std::cout << it2->first << std::endl;
				std::cout << "Methods: " << std::endl;
				std::cout << it2->second.getMethods() << std::endl;
				// std::cout << "Root: " << it2->second.getRoot() << std::endl;
				std::cout << "Index: " << std::endl;
				// for (std::vector<std::string>::iterator it3 = it2->second.getIndex().begin(); it3 != it2->second.getIndex().end(); it3++) {
					// std::cout << *it3 << std::endl;
				// }
				// std::cout << "Autoindex: " << it2->second.getAutoindex() << std::endl;
				// std::cout << "Cgi: " << it2->second.getCgi() << std::endl;
				// std::cout << "CgiPath: " << it2->second.getCgiPath() << std::endl;
				// std::cout << "UploadPath: " << it2->second.getUploadPath() << std::endl;
				// std::cout << "Return: " << it2->second.getReturn() << std::endl;
			}
		}
	};
};