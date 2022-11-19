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

void	Response::setAllowHeader(void)
{
	_response = "HTTP/1.1 405 Method Not Allowed\r\n";
	_response += "Allow: ";

	std::string		allowed;
	if (_location->methodIsAllowed(LocationConfig::GET))
		allowed += "GET, ";
	if (_location->methodIsAllowed(LocationConfig::POST))
		allowed += "POST, ";
	if (_location->methodIsAllowed(LocationConfig::DELETE))
		allowed += "DELETE";
	if (*(allowed.end() - 1) == ' ')
		allowed.erase(allowed.end() - 2, allowed.end());
	_response += allowed + "\r\n";
	std::string		body = generateErrorBody("Not allowed", "Method not allowed!");
	_response += "Content-Length: " + nbToString(body.size()) + "\r\n\r\n";
	_response += body;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, LocationConfig* loc, Request* request) :
	_config(config),
	_location(loc),
	_rqst(request),
	_response("HTTP/1.1 501 Not Implemented Yet\r\n\r\n"),
	_targetPath()
{
	std::cout << "param constructor response" << std::endl;
	if (config == NULL || loc == NULL || request == NULL)
	{
		_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
		return ;
	}
	if (request->getUri().path.size() + request->getUri().query.size() > 1024)
	{
		_response = "HTTP/1.1 414 Request URI too Long\r\n\r\n";
	}
	if (checkHost() == false)
	{
		_response = "HTTP/1.1 400 Bad Request\r\n\r\n";
		return ;
	}
	if (checkMethod() == false)
	{
		//TODO: et pue la merde passe jamais ici pour cdefone/ DELETE wtf mb root redir??
		setAllowHeader();
		return ;
	}
	setTargetPath();
	//TODO: vraiment process la rqst dans la construction?
	if (_rqst->getMethod() == "GET")
		doGET();
	else if (_rqst->getMethod() == "DELETE")
	{
		if (doDELETE(_targetPath) == 404)
			_response = generateResponse(404, "Not Found", generateErrorBody("Delete: File Not Found"));
		else
			_response = "HTTP/1.1 204 No Content\r\n\r\n";
	}
	else if (_rqst->getMethod() == "POST")
	{
		setAllowHeader();
//		doPOST();
	}
}

	//TODO: check si redirection 301
/* Set le private attribut _targetPath en fonction du path de l'URI de la request et en
fonction du root de la config. */
void	Response::setTargetPath(void)
{
	std::string		url(_rqst->getUri().path);
	_targetPath =  url.replace(0, _location->getPath().size(), _location->getRootPath());
	if (ends_with(url, '/') == true)
	{
		if (_location->isDirList() == true || _rqst->getMethod() == "DELETE")
			return ;
		_targetPath += _location->getIndexFile();
	}
}


/* Delete a file, or if its a directy: delete all elements in it before rmdir. 
Returns a status code wether access one file failed (breaks the whole operation, 
404) or entire operation succeeded (204)*/
int		Response::doDELETE(const std::string &path)
{
	typedef std::vector<std::string>	strVec;
	if (path == "./" || path == "../")
		return 204;
	if (ends_with(path, '/') == false) // si is file, remove it and stop.
	{
		if (std::remove(path.c_str()) != 0)
		{
			Logger::Error("Response::doDELETE(): remove file '%s' failed", path.c_str());
			return 404;
		}
		Logger::Info("Response::doDELETE(): file '%s' deleted", path.c_str());
		return 204;
	}
	strVec	contains = listFiles(path, false);
	for (strVec::iterator it = contains.begin(); it != contains.end(); ++it)
		it->insert(0, path);
	for (strVec::iterator it = contains.begin(); it != contains.end(); ++it)
	{
		if (doDELETE(*it) == 404)
			return 404;
	}
	std::string		dirPath(path.begin(), path.end() - 1);
	if (std::remove(dirPath.c_str()) != 0)
	{
		Logger::Error("Response::doDELETE(): remove directory '%s' failed", dirPath.c_str());
		return 404;
	}
	Logger::Info("Response::doDELETE(): directory '%s' deleted", dirPath.c_str());
	return 204;
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
		_response += "Connection: keep-alive\r\n";
		_response += "\r\n";
		_response += body;
	}
	return true;
}

void	Response::getFile(void)
{
	if (tryFile() == true)
		return ;
	ServerConfig::errors_t::const_iterator	it = _config->getErrorPaths().find(404);
	if (it == _config->getErrorPaths().end())
	{
		if (GeneralConfig::getErrors().find(404) == GeneralConfig::getErrors().end())
		{
			_response = generateResponse(404, "Not Found", "File Not Found (default)");
			return;
		}
		_response = GeneralConfig::getErrors().at(404);
		return;
	}
	std::ifstream	errFile(it->second.c_str());
	if (!errFile.is_open())
	{
		_response = generateResponse(404, "Not Found", "File Not Found (default)");
		return;
	}
	std::string		buff;
	std::string		body;
	while (std::getline(errFile, buff))
		body += buff + "\n";
	_response = generateResponse(404, "File Not Found", body);
}

void		Response::getCgi(void)
{
	
}

void		Response::doGET(void)
{
	if (targetIsDir() && _location->isDirList() == true)
	{
		std::string body = GenerateHtmlDirectory(_targetPath);
		_response = generateResponse(body);
		return ;
	}
	else if (targetIsFile())
	{
		getFile();
		return ;
	}
	else // targetIsCgi()
	{
		getCgi();
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


bool		Response::targetIsDir(void) const
{
	return (ends_with(_targetPath, '/'));
}

bool		Response::targetIsFile(void) const
{
	return !(targetIsDir() || targetIsFile());
}

bool		Response::targetIsCgi(void) const
{
	return (ends_with(_targetPath, ".php"));
}
