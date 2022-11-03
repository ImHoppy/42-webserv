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
	// std::string	_names; // Optional on subject
	map_locs	_location;
	int32_t		_maxBodySize;
	std::map<int, std::string>	_errorPages;

	public:
	
	ServerConfig();
	ServerConfig(const ServerConfig &other);
	ServerConfig &operator=(const ServerConfig &other);
	~ServerConfig();

	void	setHost(int32_t host);
	void	setPort(int16_t port);
	void	addLocation(const std::string &path, const LocationConfig &loc);
	void	setMaxBodySize(int32_t maxBodySize);
	void	addErrorPage(int code, const std::string &page);

	int32_t			getHost() const;
	int16_t			getPort() const;
	LocationConfig	getLocation(const std::string &path) const;
	map_locs		getLocations() const;
	int32_t			getMaxBodySize() const;
	std::map<int, std::string>	getErrorPages() const;

	
};