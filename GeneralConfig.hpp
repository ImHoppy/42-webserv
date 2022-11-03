#pragma once

#include <string>
#include <vector>
#include "ServerConfig.hpp"

class GeneralConfig {
	private:
	std::vector<ServerConfig>	_servers;

	public:
	GeneralConfig();
	GeneralConfig(const GeneralConfig &other);
	GeneralConfig &operator=(const GeneralConfig &other);
	~GeneralConfig();

	void	addServer(const ServerConfig &server);
};