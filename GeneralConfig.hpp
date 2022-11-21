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

#endif

