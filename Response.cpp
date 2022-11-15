#include "Response.hpp"
#include "Logger.hpp"
#include <string>

/* Default constructor */
Response::Response(void) : _config(), _location(), _rqst(), _response("HTTP/1.1 501 Not Implemented Yet\r\n\r\n") {}

/* Destructor */
Response::~Response(void) {}

/* Copy constructor */
Response::Response(const Response& src) : _config(src._config), _location(src._location),
	_rqst(src._rqst),_response(src._response) {}

/* Assignement operator */
Response &	Response::operator=(const Response& src)
{
	if (this != &src)
	{
		_config = src._config;
		_location = src._location;
		_rqst = src._rqst;
		_response = src._response;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, LocationConfig* loc, Request* request) :
	_config(config),
	_location(loc),
	_rqst(request),
	_response("HTTP/1.1 501 Not Implemented Yet\r\n\r\n")
{
	if (config == NULL || loc == NULL || request == NULL)
		return ;
	if (checkHost() == false)
		_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
	if (checkMethod() == false)
		_response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
	if (_rqst->getMethod() == "GET")
		doGET();
}

std::string	Response::getResponse(void) const
{
	return _response;
}

std::ostream&	operator<<(std::ostream& o, const Response& me)
{
	o << me.getResponse();
	return o;
}

/*	1) Reconstitue le file pathname (loca root + URL [+ index file])
	2) Essai d'ouvrir le fichier : return false si fail (file not found)
	3) Remplit le _response body avec le contenu du fichier, et set le bon Content-Lenght
*/
bool	Response::tryFile(void)
{
	//TODO: check si redirection 301
	std::string		filePathname;
	std::string		uri(_rqst->getUri().path);
	filePathname =  uri.replace(0, _location->getPath().size(), _location->getRootPath());
	Logger::Info("Replace path = %s", filePathname.c_str());
	if (endsWithSlash(_rqst->getUri().path) == true)
		filePathname += _location->getIndexFile();

	std::ifstream	file(filePathname.c_str());
	if (file.is_open() == false)
		return false;
	else
	{
		_response = "HTTP/1.1 200 OK\r\n";
		std::string		buff;
		std::string		body;
		while (getline(file, buff))
		{
			body += buff + "\n";
		}
		std::stringstream	ss;
		ss << body.size();
		_response += "Content-Length: " + ss.str() + "\r\n";
		_response += "Content-Type: text/text\r\n";
		_response += "Connection: keep-alive\r\n";
		_response += "\r\n";
		_response += body;
	}
	return true;
}

void		Response::doGET(void)
{
	if (endsWithSlash(_rqst->getUri().path) == true && _location->isDirList() == true)
	{
		std::string		filePathname;
		std::string		uri(_rqst->getUri().path);
		filePathname =  uri.replace(0, _location->getPath().size(), _location->getRootPath());
		std::string body = GenerateHtmlDirectory(filePathname);
		Logger::Info("%s %s %s", uri.c_str(), filePathname.c_str(), _location->getRootPath().c_str());
		_response = generateResponse(body);
	}
	else if (tryFile() == false)
	{
		//TODO: return l'error page correspondante au 404
		ServerConfig::errors_t::const_iterator	it = _config->getErrorPaths().find(404);
		if (it == _config->getErrorPaths().end())
		{
			if (GeneralConfig::getErrors().find(404) == GeneralConfig::getErrors().end())
			{
				_response = generateResponse(404, "Not Found", "File Not Found (default)");
			Logger::Info("Im here");
				return;
			}
			_response = GeneralConfig::getErrors().at(404);
			return;
		}
		std::ifstream	errFile(it->second.c_str());
		if (!errFile.is_open())
		{
			Logger::Warning("Here :)");
			_response = generateResponse(404, "Not Found", "File Not Found (default)");
			return;
		}
		std::string		buff;
		std::string		body;
		while (std::getline(errFile, buff))
			body += buff + "\n";
		_response = generateResponse(404, "File Not Found", body);
	}
}

bool	Response::checkHost(void) const
{
	if (_rqst->getHost() == "UNDEFINED")
		return false;
	return true;
}

bool	Response::checkMethod(void) const
{
	return _location->methodIsAllowed(_rqst->getMethod());
}

