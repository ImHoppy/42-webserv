#include "GeneralConfig.hpp"

GeneralConfig::GeneralConfig() {}

GeneralConfig::GeneralConfig(const GeneralConfig &other) {
	*this = other;
}
GeneralConfig &GeneralConfig::operator=(const GeneralConfig &other) {
	if (this != &other) {
		_servers = other._servers;
	}
	return *this;
}
GeneralConfig::~GeneralConfig() {
}

void	GeneralConfig::addServer(const ServerConfig &server) {
	_servers.push_back(server);
}

std::vector<ServerConfig> const &	GeneralConfig::getServers() const {
	return _servers;
}
