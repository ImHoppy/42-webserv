#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include <string>
# include <bitset>
# include <iostream>
# include <iomanip>


class LocationConfig {
	public:

	typedef enum { // ATTENTION little endian?? see bitset<N>::set() 
		GET = 0,
		POST = 1,
		DELETE = 2
	} http_methods; // represente la position du bit correspondant (pas la valeur)

	private:
	bool			_dirList;
	std::string		_path;
	std::string		_root;
	std::string		_indexFile;
	std::string		_redirUrl;
	std::string		_CGICmd;
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
	void	setCGICmd(const std::string &CGICmd);
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
	std::string		getCGICmd() const;
	std::string		getRedirUrl() const;
	std::string 	getIndexFile() const;
	std::bitset<3>	getMethods() const;
	bool			methodIsAllowed(http_methods method) const;
	bool			methodIsAllowed(std::string method) const;
};

std::ostream&	operator<<(std::ostream& o, const LocationConfig& me);

#endif
