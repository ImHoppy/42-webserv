#include <string>
#include "Utils.hpp"

std::string GenerateResponse(std::string fileContent)
{
	std::string response;

	response = "HTTP/1.1 200 OK\r"
		"Content-Type: text/html\r"
		"Content-Length: " + nbToString(fileContent.length()) + "\r"
		"Connection: close\r"
		"\r"
		+ fileContent;
	return response;
}

std::string GenerateResponse(int code, std::string codeMsg, std::string fileContent)
{
	std::string response;

	response = "HTTP/1.1 " + nbToString(code) + " " + codeMsg + "\r"
		"Content-Type: text/html\r"
		"Content-Length: " + nbToString(fileContent.length()) + "\r"
		"Connection: close\r"
		"\r"
		+ fileContent;
	return response;
}