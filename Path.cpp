#include "Path.hpp"
#include <sys/types.h>
#include <dirent.h>

bool	Path::isAbsolute( std::string const & path ) {
	return path[0] == '/';
}

std::vector<std::string>	Path::listFiles(std::string const & path)
{
	DIR							*dr;
	struct dirent				*en;
	std::vector<std::string>	vec_files;

	dr = opendir(path.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
			vec_files.push_back(en->d_name);
		closedir(dr);
	}
	return vec_files;
}