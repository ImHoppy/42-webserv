#include "ServerConfig.hpp"

ServerConfig::ServerConfig() :
	_host(0),
	_port(8080),
	_root("."),
	_server_names(),
	_maxBodySize(1024),
	_location(),
	_errorPages()
{}
ServerConfig::ServerConfig(const ServerConfig &other) {
	*this = other;
}
ServerConfig &ServerConfig::operator=(const ServerConfig &other) {
	if (this != &other) {
		_host = other._host;
		_port = other._port;
		_root = other._root;
		_server_names = other._server_names;
		_maxBodySize = other._maxBodySize;
		_location = other._location;
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
	_root.assign(root);
	for (map_locs::iterator it = _location.begin(); it != _location.end(); it++) {
		if (it->second.getRootPath() == "")
			it->second.setRootPath(root);
	}
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
void	ServerConfig::setServerNames(const std::vector<std::string> &server_names) {
	_server_names = server_names;
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
std::string	const &	ServerConfig::getRootPath() const {
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
std::vector<std::string> const & ServerConfig::getServerNames() const {
	return _server_names;
}
