#include "GeneralConfig.hpp"

GeneralConfig::GeneralConfig() {}

GeneralConfig::GeneralConfig(const GeneralConfig &other) {
	*this = other;
}
GeneralConfig &GeneralConfig::operator=(const GeneralConfig &other) {
	if (this != &other) {
		_configs = other._configs;
	}
	return *this;
}
GeneralConfig::~GeneralConfig() {
}

void	GeneralConfig::addServer(const ServerConfig &config) {
	_configs.push_back(config);
}

std::vector<ServerConfig> const &	GeneralConfig::getConfigs() const {
	return _configs;
}
