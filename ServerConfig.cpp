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
void	ServerConfig::setRootPath(const std::string &root) {
	_root = root;
}
void	ServerConfig::setMaxBodySize(int32_t maxBodySize) {
	_maxBodySize = maxBodySize;
}
void	ServerConfig::addLocation(const std::string &path, const LocationConfig &loc) {
	_location[path] = loc;
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
int32_t			ServerConfig::getMaxBodySize() const {
	return _maxBodySize;
}
std::string		ServerConfig::getRootPath() const {
	return _root;
}
LocationConfig	ServerConfig::getLocation(const std::string &path) const {
	return _location.at(path);
}
ServerConfig::map_locs	const & ServerConfig::getLocations() const {
	return _location;
}
std::map<int, std::string>	const & ServerConfig::getErrorPages() const {
	return _errorPages;
}
