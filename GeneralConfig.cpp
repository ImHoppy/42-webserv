#include "GeneralConfig.hpp"

GeneralConfig::GeneralConfig() {}

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
