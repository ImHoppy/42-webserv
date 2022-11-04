#pragma once

#include <string>
#include <bitset>
#include "Path.hpp"
#include "CGI.hpp"


class LocationConfig {
	public:

	typedef enum {
		GET = 0,
		POST = 1,
		DELETE = 2
	} http_methods;

	private:
	bool			_dirList;
	bool			_redir;
	bool			_CGIActive;
	std::string		_path;
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

	LocationConfig(std::string path);

	void	setDirList(bool dirList);
	void	setAlias(const std::string &alias);
	void	setMethods(const std::bitset<3> &methods);
	// TODO(mbraets): Check if used
	void	setMethod(http_methods method, bool value);
	void	addMethod(http_methods method);
	
	std::string	const &	getAlias() const;
	std::bitset<3>	getMethods() const;
	bool			getMethod(http_methods method) const;
	std::string 	getIndexFile() const;
	std::string		getPath() const;

	bool			isDirList() const;
	bool			isRedirection() const;
};