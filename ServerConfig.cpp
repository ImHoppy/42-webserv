#include "ServerConfig.hpp"

ServerConfig::ServerConfig() {}
ServerConfig::ServerConfig(const ServerConfig &other) {
	*this = other;
}
ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
	if (this != &other) {
		_host = other._host;
		_port = other._port;
		_location = other._location;
		_maxBodySize = other._maxBodySize;
		_errorPages = other._errorPages;
	}
	return *this;
}
ServerConfig::~ServerConfig() {}


/* ********************************* SETTER ********************************* */

void	ServerConfig::setHost(int32_t host) {
	_host = host;
}
void	ServerConfig::setPort(int16_t port) {
	_port = port;
}
void	ServerConfig::addLocation(const std::string &path, const LocationConfig &loc) {
	_location[path] = loc;
}
void	ServerConfig::setMaxBodySize(int32_t maxBodySize) {
	_maxBodySize = maxBodySize;
}
void	ServerConfig::addErrorPage(int code, const std::string &page) {
	_errorPages[code] = page;
}


/* ********************************* GETTER ********************************* */

int32_t			ServerConfig::getHost() const {
	return _host;
}
int16_t			ServerConfig::getPort() const {
	return _port;
}
LocationConfig	ServerConfig::getLocation(const std::string &path) const {
	return _location.at(path);
}
ServerConfig::map_locs	ServerConfig::getLocations() const {
	return _location;
}
int32_t			ServerConfig::getMaxBodySize() const {
	return _maxBodySize;
}
std::map<int, std::string>	ServerConfig::getErrorPages() const {
	return _errorPages;
}
