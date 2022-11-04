#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _dirList(false), _alias(), _methods() {}
LocationConfig::LocationConfig(const LocationConfig &other)
{
	*this = other;
}
LocationConfig &LocationConfig::operator=(const LocationConfig &other)
{
	if (this != &other)
	{
		_dirList = other._dirList;
		_redir = other._redir;
		_alias = other._alias;
		_methods = other._methods;
	}
	return *this;
}
LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(std::string path) : _dirList(false), _alias(), _methods()
{
	_path = path;
}


void	LocationConfig::setDirList(bool dirList)
{
	_dirList = dirList;
}
void	LocationConfig::setAlias(const std::string &alias)
{
	_alias = alias;
}
void	LocationConfig::setMethods(const std::bitset<3> &methods)
{
	_methods = methods;
}
void	LocationConfig::setMethod(http_methods method, bool value)
{
	_methods.set(method, value);
}
void	LocationConfig::addMethod(http_methods method)
{
	_methods.set(method, true);
}

std::string	const &		LocationConfig::getAlias() const
{
	return _alias;
}
std::bitset<3>	LocationConfig::getMethods() const
{
	return _methods;
}
bool			LocationConfig::getMethod(http_methods method) const
{
	return _methods.test(method);
}
std::string 	LocationConfig::getIndexFile() const
{
	return _indexFile;
}
std::string		LocationConfig::getPath() const
{
	return _path;
}

bool			LocationConfig::isDirList() const
{
	return _dirList;
}
bool			LocationConfig::isRedirection() const
{
	return _redir;
}
