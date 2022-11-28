#include "Utils.hpp"

std::string	generateErrorBody(std::pair<int, std::string> code)
{
	std::string html;

	html = "<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<style>"
		"img {"
		"	text-align: center;"
		"	position: absolute;"
		"	inset: 0;"
		"	margin: auto;"
		"}"
		":root {"
		"	color-scheme: light dark;"
		"	background-color: #1c1b22;"
		"}"
		"</style>"
		"<title>Error</title>"
		"</head>"
		"<body>"
		"<image src=\"https://http.cat/" + IntToStr(code.first) + "\">"
		"</body>"
		"</html>";
	return html;
}

// TODO: Si opendir fail, change code response
std::vector<std::string>	listFiles(std::string const & path, bool withDot)
{
	DIR							*dr;
	struct dirent				*en;
	std::vector<std::string>	vec_files;

	dr = opendir(path.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
		{
			std::string	name(en->d_name);
			if (en->d_type == DT_DIR)
			{
				if (withDot || (name != "." && name != ".."))
					vec_files.push_back(name + "/");
			}
			else
				vec_files.push_back(name);
		}
		closedir(dr);
	}
	return vec_files;
}

std::string	GenerateHtmlDirectory(std::string const & path)
{
	std::vector<std::string>	vec_files = listFiles(path);
	if (vec_files.empty())
		return "";
	std::string					htmlPage;
	std::vector<std::string>::iterator it;

	htmlPage = "<html><head>";
	htmlPage += "<title> /" + path + "</title>";
	htmlPage += "<style type=\"text/css\"> a {display: block; padding: .5em .75em; text-decoration: none; color: #363636; border-bottom: 1px solid #ededed;} a:hover { color: black; background-color: #a6a6a6;}</style>";
	htmlPage += "</head><body bgcolor=\"white\">";
	htmlPage += "<h1>Index of " + path + "</h1>";
	htmlPage += "<a href=\"../\">Parent Directory</a>\n";

	for (it = vec_files.begin(); it != vec_files.end(); it++)
	{
		if (startsWith(*it, "."))
			continue;
		htmlPage += "<a href=\""+ *it +"\">" + *it + "</a>\n";
	}
	htmlPage += "</body></html>\n";
	return htmlPage;
}
