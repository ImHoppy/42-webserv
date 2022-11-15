#include <string>
#include "Utils.hpp"

// OpenDir
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

std::string generateResponse(std::string fileContent)
{
	std::string response;

	response = "HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + nbToString(fileContent.length()) + "\r\n"
		"Connection: Keep-Alive\r\n"
		"\r\n"
		+ fileContent;
	return response;
}

std::string generateResponse(int code, std::string codeMsg, std::string fileContent)
{
	std::string response;

	response = "HTTP/1.1 " + nbToString(code) + " " + codeMsg + "\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + nbToString(fileContent.length()) + "\r\n"
		"Connection: Keep-Alive\r\n"
		"\r\n"
		+ fileContent;
	return response;
}

std::string	generateErrorBody(std::string title, std::string body)
{
	std::string html;

	html = "<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title>" + title + "</title>"
		"</head>"
		"<body>"
		+ body + ""
		"</body>"
		"</html>";
	return html;
}

std::string	generateErrorBody(std::string body)
{
	std::string html;

	html = "<!DOCTYPE html>"
		"<html>"
		"<head>"
		"<title>Error</title>"
		"</head>"
		"<body>"
		+ body + ""
		"</body>"
		"</html>";
	return html;
}


std::vector<std::string>	listFiles(std::string const & path)
{
	DIR							*dr;
	struct dirent				*en;
	std::vector<std::string>	vec_files;

	dr = opendir(path.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
		{
			if (en->d_type == DT_DIR)
				vec_files.push_back(std::string(en->d_name) + "/");
			else
				vec_files.push_back(en->d_name);
		}
		closedir(dr);
	}
	return vec_files;
}

bool	startsWith(std::string const & str, std::string const & start)
{
	if (str.length() < start.length())
		return false;
	return (str.compare(0, start.length(), start) == 0);
}

std::string	GenerateHtmlDirectory(std::string const & path)
{
	std::vector<std::string>	vec_files = listFiles(path);
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
	htmlPage += "</body></html>";
	return htmlPage;
}
