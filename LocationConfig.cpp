#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _dirList(false), _root(), _methods() {}
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
		_root = other._root;
		_methods = other._methods;
	}
	return *this;
}
LocationConfig::~LocationConfig() {}

void	LocationConfig::setDirList(bool dirList)
{
	_dirList = dirList;
}
void	LocationConfig::setRoot(const Path &root)
{
	_root = root;
}
void	LocationConfig::setMethods(const std::bitset<3> &methods)
{
	_methods = methods;
}
void	LocationConfig::setMethod(http_methods method, bool value)
{
	_methods.set(method, value);
}

Path			LocationConfig::getRoot() const
{
	return _root;
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

bool			LocationConfig::isDirList() const
{
	return _dirList;
}
bool			LocationConfig::isRedirection() const
{
	return _redir;
}
