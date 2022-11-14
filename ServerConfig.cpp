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

/* Doit retourner la location la plus longue qui match avec path.
	1) Si perfect match est trouve, return it.
	2) Parcourt toutes les locations et compare leur path pour trouver celle qui
	"matche le plus". La comparaison se fait par "troncon": commence au premier "/" 
	jusqu'au prochain '/' non inclus.
	3) Si Rien ne match, return NULL.*/
LocationConfig*	ServerConfig::getLocationFromPath(const std::string &path) const {
	map_locs::iterator	it = _location.find(path);
	
	if (it != _location.end())
		return &(*it);
	else // parcours toutes les locations 
	{
		std::list<LocationConfig*>	matches; //list de locations qui matchent
		std::string		loc_path = locs.getPath();
		size_t		pos;
		for (std::string::reverse_iterator rit = path.rbegin(); rit != rend(); )
		{
			for (map_locs::iterator locs = _location.begin(); locs != _location.end(); ++locs)
			{
				
			}

			rit += 
		}

	}
	return NULL;
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
	for (std::map<int, std::string>::const_iterator it2 = me.getErrorPages().begin(); it2 != me.getErrorPages().end(); it2++) {
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

bool	endsWithSlash(const std::string & path)
{
	std::string::size_type	pos;
	pos = path.find_last_of('/');
	if (pos != std::string::npos && pos == path.size() - 1)
		return true;
	return false;
}


std::string	getFileBody(const LocationConfig & loc, const std::string & rqst_path)
{
	std::string		filePathname;
	filePathname = loc.getRootPath() + rqst_path;
	if (endsWithSlash(rqst_path) == true)
		filePathname += loc.getIndexFile();
	std::cout << "File path result = " << filePathname << std::endl;
	std::ifstream	file(filePathname.c_str());
	std::string		response;
	if (file.is_open() == false)
	{
//		//TODO: return l'error page correspondante au 404
		std::cout << "file not open => not found" << std::endl;
		response = "HTTP/1.1 404 File Not Found\r\n";
	}
	else
	{
		//TODO: check si redirection 301
		std::cout << "file FOUND" << std::endl;
		response = "HTTP/1.1 200 OK\r\n";
		std::string		buff;
		std::string		body;
		while (getline(file, buff))
		{
			body += buff + "\n";
		}
		//TODO: faire une fct buildResponse() qui append ds le bon ordre les diff hdrs.
		std::stringstream	ss;
		ss << body.size();
		response += "Content-Length: " + ss.str() + "\r\n";
		response += "Content-Type: text/html\r\n";
		response += "Connection: Closed\r\n";
		response += body;
	}
	return response;
}

std::string		ServerConfig::processGet(LocationConfig* location, std::string& path)
{
	std::string		response;
	if (endsWithSlash(path) == true && location->isDirList() == true)
	{
		//TODO: print dir list; getDirList()
		response = "HTTP/1.1 200 OK\r\n";
	}
	else
	{
		response = getFileBody(location, path);
	}
	return response;
}

std::string		ServerConfig::respondRequest(Request const & rqst)
{
	std::string		response;
	std::string		host = rqst.getHost();
	if (host == "UNDEFINED")
	{
		response = "HTTP/1.1 400 Bad Request\r\n";
		return response;
	}
	std::string		path = rqst.getUri().path;
	LocationConfig*	location = getLocationFromPath(path);
	std::string		method = rqst.getMethod();
	if (location->methodIsAllowed(method) == false)
	{
		response = "HTTP/1.1 405 Method Not Allowed\r\n";
		return response;
	}
	if (method == "GET")
	{
		response = processGet(location, path);
		return response;
	}
//	return response;
	return (std::string("HTTP/1.1 200 OK\r\n"));
}
