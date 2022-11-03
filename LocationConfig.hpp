#pragma once

#include <string>
#include <bitset>
#include "Path.hpp"
#include "CGI.hpp"

class LocationConfig {
	private:
	typedef enum {
		GET = 0,
		POST = 1,
		DELETE = 2
	} http_methods;

	bool			_dirList;
	bool			_redir;
	bool			_CGIActive;
	std::string		_CGIPath;
	CGI				_CGI;
	std::string		_redirUrl;
	std::string		_indexFile;
	std::string		_alias;
	std::bitset<3>	_methods;

	
	public:
	LocationConfig();
	LocationConfig(const LocationConfig &other);
	LocationConfig &operator=(const LocationConfig &other);
	~LocationConfig();

	void	setDirList(bool dirList);
	void	setAlias(const std::string &alias);
	void	setMethods(const std::bitset<3> &methods);
	void	setMethod(http_methods method, bool value);
	
	std::string	const &	getAlias() const;
	std::bitset<3>	getMethods() const;
	bool			getMethod(http_methods method) const;
	std::string 	getIndexFile() const;

	bool			isDirList() const;
	bool			isRedirection() const;
};