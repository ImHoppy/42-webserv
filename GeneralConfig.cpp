#include "GeneralConfig.hpp"
#include "Utils.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

GeneralConfig::GeneralConfig() {
	if (GeneralConfig::_errorPages.empty())
	{
		std::fstream error_name("error.conf", std::fstream::in);
		if (error_name.is_open()) {
			std::string line;
			while (std::getline(error_name, line)) {
				
				std::istringstream ss(line);
				std::string StrCode;
				std::string phrase;
				std::getline(ss, StrCode, '\t');
				std::getline(ss, phrase);
				int code = StrToInt(StrCode);
				std::string response =  generateResponse(code, phrase, generateErrorBody(StrCode + " | " + phrase, phrase));
				GeneralConfig::_errorPages.insert(std::make_pair(code, response));
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

std::map<int, std::string>	GeneralConfig::_errorPages;

const std::map<int, std::string>		GeneralConfig::getErrors(void)
{
	return GeneralConfig::_errorPages;
}
