#ifndef PATH_HPP
# define PATH_HPP

# include <string>
# include <vector>
# include <sys/types.h>
# include <dirent.h>

class Path : public std::string {
	private:
	std::string	_rootPath;
	public:
	// using std::string::basic_string;

	void			setRoot(std::string const & path);

	std::string const &	getFilename() const;
	std::string const &	getFullpath();
	std::string const &	joinRoot() const;
	
	static bool				isAbsolute( std::string const & path );
	static std::vector<std::string>	listFiles( std::string const & path );
};

#endif

