#pragma once

#include <string>
#include <vector>
#include "ServerConfig.hpp"
#include <iostream>
#include <map>

class GeneralConfig {
	private:
	std::vector<ServerConfig>	_servers;
	static std::map<int, std::string>	_errors;

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
			std::cout << "Root: " << it->getRootPath() << std::endl;
			std::cout << "MaxBodySize: " << it->getMaxBodySize() << std::endl;
			std::cout << "ErrorPages: " << std::endl;
			for (std::map<int, std::string>::const_iterator it2 = it->getErrorPages().begin(); it2 != it->getErrorPages().end(); it2++) {
				std::cout << "\t" << it2->first << " " << it2->second << std::endl;
			}
			std::cout << "Locations: " << std::endl;
			for (std::map<std::string, LocationConfig>::const_iterator it2 = it->getLocations().begin(); it2 != it->getLocations().end(); it2++) {
				std::cout << it2->first << std::endl;
				std::cout << "\tRoot: " << it2->second.getRootPath() << std::endl;
				std::cout << "\tMethods: " << it2->second.getMethods() << std::endl;
				std::cout << "\tRedirect: " << it2->second.getRedirUrl() << std::endl;
				std::cout << "\tCgi: " << std::boolalpha <<it2->second.isCGIActive() << std::endl;
				std::cout << "\tCgiPath: " << it2->second.getCGIPath() << std::endl;
				// std::cout << "UploadPath: " << it2->second.getUploadPath() << std::endl;
				// std::cout << "Return: " << it2->second.getReturn() << std::endl;
			}
		}
	};
};
