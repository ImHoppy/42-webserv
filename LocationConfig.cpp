#include "LocationConfig.hpp"

LocationConfig::LocationConfig() : _dirList(false), _CGIActive(false) {}
LocationConfig::LocationConfig(const LocationConfig &other)
{
	*this = other;
}
LocationConfig &LocationConfig::operator=(const LocationConfig &other)
{
	if (this != &other)
	{
		_dirList = other._dirList;
		_CGIActive = other._CGIActive;
		_path = other._path;
		_CGIPath = other._CGIPath;
		_CGI = other._CGI;
		_redirUrl = other._redirUrl;
		_indexFile = other._indexFile;
		_root = other._root;
		_methods = other._methods;
	}
	return *this;
}
LocationConfig::~LocationConfig() {}

LocationConfig::LocationConfig(std::string path)  : _dirList(false), _CGIActive(false),_path(path)
{}

void	LocationConfig::setPath(const std::string & path)
{
	_path = path;
}
void	LocationConfig::setDirList(bool dirList)
{
	_dirList = dirList;
}
void	LocationConfig::setRootPath(const std::string &path)
{
	_root = path;
}
void	LocationConfig::setIndexFile(const std::string &indexFile)
{
	_indexFile = indexFile;
}
void	LocationConfig::setRedirUrl(const std::string &url)
{
	_redirUrl = url;
}
void	LocationConfig::setCGIPath(const std::string &CGIPath)
{
	_CGIPath = CGIPath;
	if (_CGIPath != "")
	_CGIActive = not _CGIPath.empty();
}
void	LocationConfig::setCGICmd(const std::string &CGICmd)
{
	_CGIPath = CGICmd;
	// _CGI.setCmd(CGIPath);
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


std::string 	LocationConfig::getIndexFile() const
{
	return _indexFile;
}
std::string		LocationConfig::getRootPath() const
{
	return _root;
}
bool			LocationConfig::isDirList() const
{
	return _dirList;
}
bool			LocationConfig::isRedirection() const
{
	return (not _redirUrl.empty());
}
std::bitset<3>	LocationConfig::getMethods() const
{
	return _methods;
}
bool			LocationConfig::getMethod(http_methods method) const
{
	return _methods.test(method);
}
#include <iostream>
bool	LocationConfig::isEmpty() const
{
	return (_dirList == false && _CGIActive == false && _CGIPath.empty() && _indexFile.empty() && _root.empty() && _methods.none());
}