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
	std::vector<ServerConfig>			_configs;

	public:
	GeneralConfig();
	GeneralConfig(const GeneralConfig &other);
	GeneralConfig &operator=(const GeneralConfig &other);
	~GeneralConfig();

	void	addServer(const ServerConfig &config);
	std::vector<ServerConfig> const &	getConfigs() const;
};

#endif

