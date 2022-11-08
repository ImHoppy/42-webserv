#include "GeneralConfig.hpp"
#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

GeneralConfig::GeneralConfig() {
	if (GeneralConfig::_error_status.empty())
	{
		std::fstream error_name("error.conf", std::fstream::in);
		if (error_name.is_open()) {
			std::string line;
			while (std::getline(error_name, line)) {
				
				std::istringstream ss(line);
				std::string key;
				std::string value;
				std::getline(ss, key, '\t');
				std::getline(ss, value);

				GeneralConfig::_error_status.insert(std::make_pair(StrToInt(key), value));
			}
		}
	}
}
GeneralConfig::GeneralConfig(const GeneralConfig &other) {
	// std::cout << "GeneralConfig copy constructor called" << std::endl;
	*this = other;
}
GeneralConfig &GeneralConfig::operator=(const GeneralConfig &other) {
	// std::cout << "GeneralConfig assignation operator called" << std::endl;
	if (this != &other) {
		_servers = other._servers;
	}
	return *this;
}
GeneralConfig::~GeneralConfig() {
	// std::cout << "GeneralConfig destructor called" << std::endl;
}

void	GeneralConfig::addServer(const ServerConfig &server) {
	_servers.push_back(server);
}

std::vector<ServerConfig> const &	GeneralConfig::getServers() const {
	return _servers;
}

std::map<int, std::string>	GeneralConfig::_error_status;
