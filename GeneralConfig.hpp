#pragma once

#include <string>
#include <vector>
#include "ServerConfig.hpp"
#include <iostream>
#include <map>

class GeneralConfig {
	private:
	std::vector<ServerConfig>			_servers;
	static std::map<int, std::string>	_errorPages;

	public:
	GeneralConfig();
	GeneralConfig(const GeneralConfig &other);
	GeneralConfig &operator=(const GeneralConfig &other);
	~GeneralConfig();

	static const std::map<int, std::string>		getErrors(void);
	void	addServer(const ServerConfig &server);
	std::vector<ServerConfig> const &	getServers() const;
};
