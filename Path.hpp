#pragma once

#include <string>
#include <vector>

class Path : public std::string {
	private:
	std::string	_rootPath;
	public:
	using std::string::basic_string;

	std::string &	getFilename() const;
	std::string &	getFullpath();
	std::string &	joinRoot() const;
	
	static bool				isAbsolute( std::string const & path );
	static std::vector<std::string>	listFiles( std::string const & path );
};