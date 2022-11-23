#include "Response.hpp"

/* Default constructor */
Response::Response(void) :
	_config(), _location(), _rqst(), _client(), 
	_targetPath(),
	_cgi(),
	_code(std::make_pair(501, "Not Implemented")),
	_headers(),
	_body(),
	_response(),
	_readData()
{}

/* Destructor */
Response::~Response(void) {
	if (_readData.buffer != NULL)
		delete[] _readData.buffer;
	if (_readData.file.is_open())
		_readData.file.close();
}

/* Copy constructor */
Response::Response(const Response& src) {this->operator=(src);}

/* Assignement operator */
Response &	Response::operator=(const Response& src)
{
	if (this != &src)
	{
		_config = src._config;
		_location = src._location;
		_rqst = src._rqst;
		_client = src._client;
		_targetPath = src._targetPath;
		_cgi = src._cgi;
		_code = src._code;
		_headers = src._headers;
		_body = src._body;
		_response = src._response;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, LocationConfig* loc, Request* request, Client* client) :
	_config(config),
	_location(loc),
	_rqst(request),
	_client(client),
	_targetPath(),
	_cgi(),
	_code(std::make_pair(501, "Not Implemented")),
	_headers(),
	_body(),
	_response(),
	_readData()
{
	_headers["Connection"] = "keep-alive";
	if (config == NULL || loc == NULL || request == NULL)
	{
		_code = std::make_pair(400, "Bad Request");
		return ;
	}
	if (request->getUri().path.size() + request->getUri().query.size() > 1024)
	{
		_code = std::make_pair(414, "Request-URI Too Long");
		return ;
	}
	if (checkHost() == false)
	{
		_code = std::make_pair(400, "Bad Request");
		return ;
	}
	if (checkMethod() == false)
	{
		setAllowHeader();
		return ;
	}
	setTargetPath();
	if (_rqst->getMethod() == "GET")
	{
		doGET();
		return ;
	}
	else if (_rqst->getMethod() == "DELETE")
	{
		if (doDELETE(_targetPath) == 404)
			_code = std::make_pair(404, "Not Found");
		else
			_code = std::make_pair(204, "No Content");
	}
	else if (_rqst->getMethod() == "POST")
	{
		doPOST();
	}
}


void	Response::setAllowHeader(void)
{
	std::string		allowed;
	if (_location->methodIsAllowed(LocationConfig::GET))
		allowed += "GET, ";
	if (_location->methodIsAllowed(LocationConfig::POST))
		allowed += "POST, ";
	if (_location->methodIsAllowed(LocationConfig::DELETE))
		allowed += "DELETE";
	if (*(allowed.end() - 1) == ' ')
		allowed.erase(allowed.end() - 2, allowed.end());
	_headers["Allow"] = allowed;
	
	_code = std::make_pair(405,  "Method Not Allowed");
}

bool	Response::openPageError(std::string path)
{

	std::ifstream	errFile(path.c_str());
	if (!errFile.is_open())
		return false;
	std::string		buff;
	while (std::getline(errFile, buff))
		_body += buff + "\n";
	return true;
}

void	Response::generateBodyError()
{
	if (_code.first >= 400)
	{
		ServerConfig::errors_t::const_iterator	it = _config->getErrorPaths().find(_code.first);

		if (it == _config->getErrorPaths().end() || not openPageError(it->second))
		{
			_body = generateErrorBody(_code);
			_headers["Connection"] = "close";
		}
		_headers["Content-Length"] = IntToStr(_body.size());
	}
}

void	Response::generateResponse(void)
{
	generateBodyError();
	// if (_code.first == 200 && _body.empty() && _headers["Content-Length"][0] == '0' )
		// _code = std::make_pair(204, "No Content");
	_response = "HTTP/1.1 " + IntToStr(_code.first) + " " + _code.second + CLRF;

	headers_t::const_iterator	it;
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += it->first + ": " + it->second + CLRF;

	// if (_body.size() > 0)
		// _response += "Content-Length: " + IntToStr(_body.size()) + CLRF;
	
	_response += CLRF;

	_response += _body;
}

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

void	Response::upload(void)
{
	//TODO
	_response = "HTTP/1.1 200 OK\r\n\r\n";
}

void	Response::doPOST(void)
{
	std::string		multipart = "multipart/form-data";
	Logger::Info("doPOST() entered");
	Request::headers_t	headers = _rqst->getHeaders();
	Request::headers_t::const_iterator	type = headers.find("Content-Type");
	if (type == headers.end())
	{
		_code = std::make_pair(501, "Not Implemented");
		return ;
	}
	if (type->second.compare(0, multipart.size(), multipart) == true) // upload
	{
		Logger::Info("is multiform()");
		upload();
		return ;
	}
	if (type->second == "application/x-www-form-urlencoded")
	{
		Logger::Info("is URL encoded");
//		cgiPost();
		return ;
	}
	_code = std::make_pair(400, "Bad Request");
}

std::string	Response::getResponse(void) const
{
	return _response;
}

Response::ReadData const & Response::getReadData(void) const
{
	return _readData;
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
	if (_readData.buffer == NULL)
	{
		_readData.file.open(_targetPath.c_str());
		if (_readData.file.is_open() == false)
			return false;

		// get length of _readData.file:
		_readData.file.seekg(0, _readData.file.end);
		if (not _readData.file.good())
			return false; // TODO: error : _readData.file is directory
		int length = _readData.file.tellg();
		_headers["Content-Length"] = IntToStr(length);
		_readData.file.seekg(0, _readData.file.beg);

		_readData.buffer = new char [1024];
		bzero(_readData.buffer, 1024);

		_readData.file.read(_readData.buffer, 1024);
		_readData.read_bytes = _readData.file.gcount();

		_body.assign(_readData.buffer, _readData.read_bytes);

		if (_readData.file.eof())
		{
			Logger::Error("EOF1");
			_readData.file.close();
			_readData.status = EOF_FILE;
		}
		else
			_readData.status = READY_READ;
	}
	else
	{
		// if (not _readData.file.is_open()) return true;
		Logger::Info("Respond - Read Data");
		bzero(_readData.buffer, 1024);
		_readData.file.read(_readData.buffer, 1024);
		_readData.read_bytes = _readData.file.gcount();
		if (_readData.file.eof())
		{
			Logger::Error("EOF2");
			_readData.file.close();
			_readData.status = EOF_FILE;
		}
		else
		{
			if (_readData.file.fail())
				throw std::runtime_error("read file failed");
			_readData.status = READY_SEND;
		}
	}
	return true;
}

/* Set le vector d'environnement variables (RFC 3875) */
void		Response::setCgiEnv(void)
{
	Request::headers_t		headers = _rqst->getHeaders();
	Request::headers_t::const_iterator	not_found = headers.end();

	Request::headers_t::const_iterator	found = headers.find("Authorization");
	if (found != not_found)
	{
		_cgi.addVarToEnv("AUTH_TYPE" + found->second);
	}
	found = headers.find("Content-Type");
	if (found != not_found)
	{
		_cgi.addVarToEnv("CONTENT_TYPE" + found->second);
	}
	found = headers.find("Content-Length");
	if (found != not_found)
	{
		_cgi.addVarToEnv("CONTENT_LENGTH" + found->second);
	}
	_cgi.addVarToEnv("REDIRECT_STATUS=true");
	_cgi.addVarToEnv("GATEWAY_INTERFACE=CGI/0.1");
	_cgi.addVarToEnv("PATH_INFO=/"); // ou si myscript.php/this/is/pathinfo?query
	_cgi.addVarToEnv("QUERY_STRING=" + _rqst->getUri().query);
//	_cgi.addVarToEnv("REMOTE_ADDR=" + inet_ntoa(client->addr())); //IP
	_cgi.addVarToEnv("REQUEST_METHOD=" + _rqst->getMethod());
	_cgi.addVarToEnv("SCRIPT_FILENAME=cgi-bin/" + _rqst->getUri().path);
	_cgi.addVarToEnv("SCRIPT_NAME=" + _rqst->getUri().path);
	_cgi.addVarToEnv("SERVER_NAME=" + _config->getServerNames()[0]);
	_cgi.addVarToEnv("SERVER_PORT=" + IntToStr(_config->getPort()));
	_cgi.addVarToEnv("SERVER_PROTOCOL=HTTP/1.1");
	_cgi.addVarToEnv("SERVER_SOFTWARE=WebServ");
	//TODO: RFC 3875 parle meta-variables: export tous les headers de la request
}

/* CGI GET quand par exemple html fomr pour submit une image qu'on veut afficher
dans le navigateur */
void		Response::phpCgiGet(void)
{
	setCgiEnv();
	if (_cgi.launch() == -1)
	{
		_code = std::make_pair(500, "Internal Server Error");
		return ;
	}
	//TODO: alors soit disant faut ajouter les "locaux" fds a epoll, genre
	// meme pour lire le pipe bah faut passer par epoll... et pour les error files
	if (readFromCgi() == -1)
		_code = std::make_pair(500, "Internal Server Error");
	else
		_code = std::make_pair(200, "OK");
}

/* Remplit le _body depuis les datas read from le pipe du CGI. */
int		Response::readFromCgi(void)
{
	int	fd = _cgi.getReadPipe();
	char buf[BUFFSIZE];
	ssize_t	nbread = read(fd, buf, BUFFSIZE - 1);
	if (nbread == -1)
	{
		Logger::Error("Response::phpCgiGet() read() failed");
		return -1;
	}
	buf[nbread] = 0;
	_body += buf;
	while (nbread == BUFFSIZE - 1)
	{
		nbread = read(fd, buf, BUFFSIZE - 1);
		if (nbread == -1)
		{
			Logger::Error("Response::phpCgiGet() read() failed");
			return -1;
		}
		buf[nbread] = 0;
		_body += buf;
	}
	close(fd);
	return (nbread);
}

void		Response::doGET(void)
{
	
	if (targetIsDir() && _location->isDirList() == true)
	{
		_body = GenerateHtmlDirectory(_targetPath);
		_code = std::make_pair(200, "OK");
		if (_body.empty())
			_code = std::make_pair(501, "Internal Server Error");
		_headers["Content-Length"] = IntToStr(_body.size());
		return ;
	}
	else if (targetIsFile())
	{
		if (tryFile())
			_code = std::make_pair(200, "OK");
		else
			_code = std::make_pair(404, "Not Found");
		return ;
	}
	else // targetIsCgi()
	{
		// NOTE: Add _code = std::make_pair(200, "OK");
		phpCgiGet();
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
	return !(targetIsDir() || targetIsCgi());
}

bool		Response::targetIsCgi(void) const
{
	return (ends_with(_targetPath, ".php"));
}
