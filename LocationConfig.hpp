#pragma once

#include <string>
#include <bitset>
#include "Path.hpp"
#include "CGI.hpp"


class LocationConfig {
	public:

	typedef enum {
		GET = 1,
		POST = 2,
		DELETE = 4
	} http_methods;

	private:
	bool			_dirList;
	bool			_CGIActive;
	std::string		_path;
	std::string		_root;
	std::string		_indexFile;
	std::string		_redirUrl;
	std::string		_CGIPath;
	// CGI				_CGI;
	std::bitset<3>	_methods;

	
	public:
	LocationConfig();
	LocationConfig(const LocationConfig &other);
	LocationConfig &operator=(const LocationConfig &other);
	~LocationConfig();

	LocationConfig(std::string path);

	void	setDirList(bool dirList);
	void	setPath(const std::string & path);
	void	setRootPath(const std::string &path);
	void	setIndexFile(const std::string &indexFile);
	void	setRedirUrl(const std::string &url);
	void	setCGIPath(const std::string &CGIPath);
	void	setCGICmd(const std::string &CGIPath);
	void	setMethods(const std::bitset<3> &methods);
	// TODO(mbraets): Check if used
	void	setMethod(http_methods method, bool value);
	void	addMethod(http_methods method);
	
	bool			isEmpty() const;
	bool			isDirList() const;
	bool			isRedirection() const;
	bool			isCGIActive() const;
	std::string		getPath() const;
	std::string		getRootPath() const;
	std::string		getCGIPath() const;
	std::string		getRedirUrl() const;
	std::string 	getIndexFile() const;
	std::bitset<3>	getMethods() const;
	bool			getMethod(http_methods method) const;

	bool	methodIsAllowed(std::string method);
};
