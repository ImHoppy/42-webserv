#ifndef GENERALCONFIG_HPP
# define GENERALCONFIG_HPP

# include <string>
# include <vector>
# include <map>
# include <iostream>
# include <fstream>
# include <sstream>
# include "ServerConfig.hpp"
# include "Utils.hpp"

class ServerConfig;

class GeneralConfig {
	private:
	std::vector<ServerConfig>			_servers;

	public:
	GeneralConfig();
	GeneralConfig(const GeneralConfig &other);
	GeneralConfig &operator=(const GeneralConfig &other);
	~GeneralConfig();

	void	addServer(const ServerConfig &server);
	std::vector<ServerConfig> const &	getServers() const;
};

#endif

