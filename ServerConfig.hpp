#pragma once

#include <string>
#include <vector>
#include <map>
#include "LocationConfig.hpp"
#include <stdint.h>

class ServerConfig {
	private:
	
	typedef typename std::map<std::string, LocationConfig> map_locs;
	
	int32_t		_host;
	int16_t		_port;
	std::string	_root;
	// std::string	_names; // Optional on subject
	int32_t		_maxBodySize;
	map_locs	_location;
	std::map<int, std::string>	_errorPages;

	public:
	
	ServerConfig();
	ServerConfig(const ServerConfig &other);
	ServerConfig &operator=(const ServerConfig &other);
	~ServerConfig();

	void	setHost(int32_t host);
	void	setPort(int16_t port);
	void	setRootPath(const std::string &root);
	void	setMaxBodySize(int32_t maxBodySize);
	void	addLocation(const std::string &path, const LocationConfig &loc);
	void	addErrorPage(int code, const std::string &page);

	int32_t			getHost() const;
	int16_t			getPort() const;
	int32_t			getMaxBodySize() const;
	std::string		getRootPath() const;
	LocationConfig	getLocation(const std::string &path) const;
	map_locs	const &	getLocations() const;
	std::map<int, std::string>	const & getErrorPages() const;

	
};