#include "Response.hpp"
#include "Logger.hpp"
#include <string>

/* Default constructor */
Response::Response(void) : _config(), _location(), _rqst(), _response("HTTP/1.1 501 Not Implemented Yet\r\n\r\n"), _targetPath() {}

/* Destructor */
Response::~Response(void) {}

/* Copy constructor */
Response::Response(const Response& src) : _config(src._config), _location(src._location),
	_rqst(src._rqst), _response(src._response), _targetPath(src._targetPath) {}

/* Assignement operator */
Response &	Response::operator=(const Response& src)
{
	if (this != &src)
	{
		_config = src._config;
		_location = src._location;
		_rqst = src._rqst;
		_response = src._response;
		_targetPath = src._targetPath;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, LocationConfig* loc, Request* request) :
	_config(config),
	_location(loc),
	_rqst(request),
	_response("HTTP/1.1 501 Not Implemented Yet\r\n\r\n"),
	_targetPath()
{
	if (config == NULL || loc == NULL || request == NULL)
		return ;
	if (checkHost() == false)
		_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
	if (checkMethod() == false)
		_response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
	setTargetPath();
	//TODO: vraiment process la rqst dans la construction?
	if (_rqst->getMethod() == "GET")
		doGET();
	else if (_rqst->getMethod() == "DELETE")
	{
		doDELETE(_targetPath);
		_response = "HTTP/1.1 204 No Content\r\n\r\n";
	}
	else if (_rqst->getMethod() == "POST")
		doPOST();
}

/* Set le private attribut _targetPath en fonction du path de l'URI de la request et en
fonction du root de la config. */
void	Response::setTargetPath(void)
{
	std::string		url(_rqst->getUri().path);
	if (endsWithSlash(url) == true && (_location->isDirList() == true || _rqst->getMethod() == "DELETE"))
	{
		_targetPath =  url.replace(0, _location->getPath().size(), _location->getRootPath());
		return ;
	}
	//TODO: check si redirection 301
	_targetPath =  url.replace(0, _location->getPath().size(), _location->getRootPath());
	if (endsWithSlash(url) == true)
		_targetPath += _location->getIndexFile();
}


/* Delete a file, or if its a directy: delete all elements in it before rmdir. */
void	Response::doDELETE(const std::string &path)
{
	if (path == "./" || path == "../")
		return ;
	if (endsWithSlash(path) == false) // si is file, remove it and stop.
	{
		if (std::remove(path.c_str()) != 0)
		{
			Logger::Error("Response::doDELETE(): remove file '%s' failed", path.c_str());
			return ;
		}
		Logger::Info("Response::doDELETE(): file '%s' deleted", path.c_str());
		return ;
	}
	std::vector<std::string>	contains = listFiles(path, false);
	for (std::vector<std::string>::iterator it = contains.begin(); it != contains.end(); ++it)
	{
		it->insert(0, path);
	}
	for (std::vector<std::string>::iterator it = contains.begin(); it != contains.end(); ++it)
	{
		doDELETE(*it);
	}
	std::string		dirPath(path.begin(), path.end() - 1);
	if (std::remove(dirPath.c_str()) != 0)
	{
		Logger::Error("Response::doDELETE(): remove directory '%s' failed", dirPath.c_str());
		return ;
	}
	Logger::Info("Response::doDELETE(): directory '%s' deleted", dirPath.c_str());
	return ;
}

void	Response::doPOST(void)
{
	//TODO
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

/*
	1) Essai d'ouvrir le fichier : return false si fail (file not found)
	2) Remplit le _response body avec le contenu du fichier, et set le bon Content-Lenght
*/
bool	Response::tryFile(void)
{
	Logger::Info("Response::tryFile() path = %s", _targetPath.c_str());
	std::ifstream	file(_targetPath.c_str());
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
		//TODO: MIME 
//		_response += "Content-Type: text/plain\r\n";// + getFileExtension(_targetPath) + "\r\n";
		_response += "Connection: keep-alive\r\n";
		_response += "\r\n";
		_response += body;
	}
	return true;
}

void		Response::doGET(void)
{
	if (endsWithSlash(_targetPath) == true && _location->isDirList() == true)
	{
		std::string body = GenerateHtmlDirectory(_targetPath);
		_response = generateResponse(body);
	}
	else if (tryFile() == false)
	{
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

