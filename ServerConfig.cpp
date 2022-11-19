#include "ServerConfig.hpp"

ServerConfig::ServerConfig() :
	_host(0),
	_port(8080),
	_root("."),
	_server_names(),
	_maxBodySize(1024),
	_location(),
	_errorPath()
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
		_errorPath = other._errorPath;
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
void	ServerConfig::addErrorPaths(int code, const std::string &page) {
	_errorPath[code] = page;
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

/* Doit retourner la location la plus longue qui match avec url.
	0) Si extension == ".php" on cherche la loc .php
	1) Si perfect match est trouve, return it.
	2) Parcourt toutes les locations et compare leur url pour trouver celle qui
	"matche le plus". La comparaison se fait par "troncon": commence au premier "/" 
	jusqu'au prochain '/' non inclus.
	3) Si Rien ne match, return NULL.*/
LocationConfig*	ServerConfig::getLocationFromUrl(const std::string &url)
{
	if (ends_with(url, ".php") == true)
	{
		for (map_locs::iterator locs = _location.begin(); locs != _location.end(); ++locs)
		{
			if (locs->first == ".php")
				return &(locs->second);
		}
		return NULL;
	}
	std::string::size_type found;
	std::string::size_type start_search = url.size();
	while (start_search != std::string::npos)
	{
		for (map_locs::iterator locs = _location.begin(); locs != _location.end(); ++locs)
		{
			found = url.rfind(locs->second.getPath(), start_search);
			if (found == 0)
				return &(locs->second);
		}
		start_search = url.find_last_of('/', start_search - 1);
	}
	return NULL;
}
ServerConfig::map_locs	const & ServerConfig::getLocations() const {
	return _location;
}
std::map<int, std::string>	const & ServerConfig::getErrorPaths() const {
	return _errorPath;
}
std::vector<std::string> const & ServerConfig::getServerNames() const {
	return _server_names;
}

std::ostream&	operator<<(std::ostream& o, const ServerConfig& me)
{
	o << "Host: " << me.getHost() << std::endl;
	o << "Port: " << me.getPort() << std::endl;
	o << "Root: " << me.getRootPath() << std::endl;
	o << "Server Name: " << std::endl;
	for (std::vector<std::string>::const_iterator it2 = me.getServerNames().begin(); it2 != me.getServerNames().end(); it2++) {
		o << "\t" << *it2 << std::endl;
	}
	o << "MaxBodySize: " << me.getMaxBodySize() << std::endl;
	o << "ErrorPages: " << std::endl;
	for (std::map<int, std::string>::const_iterator it2 = me.getErrorPaths().begin(); it2 != me.getErrorPaths().end(); it2++) {
		o << "\t" << it2->first << " " << it2->second << std::endl;
	}
	o << "Locations: " << std::endl;
	for (std::map<std::string, LocationConfig>::const_iterator it2 = me.getLocations().begin(); it2 != me.getLocations().end(); it2++) {
		o << it2->first << std::endl;
		o << "\tRoot: " << it2->second.getRootPath() << std::endl;
		o << "\tMethods: " << it2->second.getMethods() << std::endl;
		o << "\tRedirect: " << it2->second.getRedirUrl() << std::endl;
		o << "\tCgi: " << std::boolalpha <<it2->second.isCGIActive() << std::endl;
		o << "\tCgiPath: " << it2->second.getCGIPath() << std::endl;
	}
	return o;
}
