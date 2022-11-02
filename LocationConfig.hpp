#pragma once

#include <string>
#include <bitset>
#include "Path.hpp"

class LocationConfig {
	private:
	
	bool			_dirList;
	Path			_root;
	std::bitset<4>	_methods;
	
	public:
	LocationConfig();
	LocationConfig(const LocationConfig &other);
	LocationConfig &operator=(const LocationConfig &other);
	~LocationConfig();

	void	setDirList(bool dirList);
	void	setRoot(const Path &root);
	void	setMethods(const std::bitset<4> &methods);
	
	Path			getRoot() const;
	std::bitset<4>	getMethods() const;

	bool			isDirList() const;
	bool			isRedirection() const;
};