#include "Response.hpp"

/* Default constructor */
Response::Response(void) :
	_rqst(),
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
		_rqst = src._rqst;
		_cgi = src._cgi;
		_code = src._code;
		_headers = src._headers;
		_body = src._body;
		_response = src._response;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(Request* rqst) :
	_rqst(rqst),
	_cgi(),
	_code(std::make_pair(501, "Not Implemented")),
	_headers(),
	_body(),
	_response(),
	_readData()
{
	_headers["Connection"] = "keep-alive";
	if (_rqst == NULL)
	{
		_code = std::make_pair(400, "Bad Request");
		return ;
	}
	if (_rqst->getUri().path.size() + _rqst->getUri().query.size() > 1024)
	{
		_code = std::make_pair(414, "Request-URI Too Long");
		return ;
	}
	if (checkHost() == false)
	{
		_code = std::make_pair(400, "Bad Request");
		return ;
	}
	if (not _rqst->getLocation()->getRedirUrl().empty())
	{
		_code = std::make_pair(302, "Found");
		Logger::Info("Redirection to " + _rqst->getLocation()->getRedirUrl());
		_headers["Location"] = _rqst->getLocation()->getRedirUrl();
		return ;
	}
	if (checkMethod() == false)
	{
		setAllowHeader();
		return ;
	}
	doMethod();
}

bool	Response::checkBodySize(void)
{
	return (StrToInt(_rqst->getContentLength()) > _rqst->getConfig()->getMaxBodySize());
}

/* "Launch" le process de la response. */
void	Response::doMethod(void)
{
	if (_rqst->getMethod() == "GET")
	{
		doGET();
	}
	else if (_rqst->getMethod() == "DELETE")
	{
		if (doDELETE(_rqst->getTargetPath()) == 404)
			_code = std::make_pair(404, "Not Found");
		else
			_code = std::make_pair(204, "No Content");
	}
	else if (_rqst->getMethod() == "POST")
	{
		if (checkBodySize() == false)
		{
			_code = std::make_pair(413, "Request Entity Too Large");
			return ;
		}
		doPOST();
	}

}

void	Response::setAllowHeader(void)
{
	LocationConfig*	loc = _rqst->getLocation();
	std::string		allowed;
	if (loc->methodIsAllowed(LocationConfig::GET))
		allowed += "GET, ";
	if (loc->methodIsAllowed(LocationConfig::POST))
		allowed += "POST, ";
	if (loc->methodIsAllowed(LocationConfig::DELETE))
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
		ServerConfig::errors_t::const_iterator	it = _rqst->getConfig()->getErrorPaths().find(_code.first);

		if (it == _rqst->getConfig()->getErrorPaths().end() || not openPageError(it->second))
		{
			_body = generateErrorBody(_code);
			_headers["Connection"] = "close";
		}
	}
}

void	Response::generateResponse(void)
{
	generateBodyError();
	if (_headers.find("Content-Length") == _headers.end())
		_headers["Content-Length"] = IntToStr(_body.size());
	// if (_code.first == 200 && _body.empty() && _headers["Content-Length"][0] == '0' )
		// _code = std::make_pair(204, "No Content");
	_response = "HTTP/1.1 " + IntToStr(_code.first) + " " + _code.second + CLRF;

	headers_t::const_iterator	it;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		_response += it->first + ": " + it->second + CLRF;
	}

	// if (_body.size() > 0)
		// _response += "Content-Length: " + IntToStr(_body.size()) + CLRF;
	
	_response += CLRF;

	_response += _body;
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


void	Response::phpCgiPost(void)
{
	setCgiEnv();
	_cgi.initFileIn(_rqst->getUploadFile());
	_cgi.initFileOut();
	if (_cgi.launch() == -1)
	{
		_code = std::make_pair(500, "Internal Server Error");
		return ;
	}
	if (readFromCgi() == -1)
		_code = std::make_pair(500, "Internal Server Error");
	else
		_code = std::make_pair(200, "OK");
}

void Response::UploadMultipart(void)
{
	Request::headers_t::const_iterator it = _rqst->getHeaders().find("Content-Type");
	if (it == _rqst->getHeaders().end() || it->second.find("boundary=") == std::string::npos)
	{
		_code = std::make_pair(400, "Bad Request");
		return ;
	}
	std::string boundary = it->second;
	boundary.erase(0, boundary.find("boundary=") + 9);
	boundary.insert(0, "--");

	std::ifstream file(_rqst->getUploadFile().c_str());
	if (not file.is_open())
	{
		_code = std::make_pair(500, "Internal Server Error");
		return ;
	}
	std::string line;
	std::string filename;
	std::string content;

	while (std::getline(file, line))
	{
		// NOTE: Cause error if file contains Content-Disposition ?
		if (startsWith(line, "Content-Disposition"))
		{
			if (line.find("filename") != std::string::npos)
			{
				filename = line.substr(line.find("filename") + 10);
				filename = filename.substr(0, filename.find("\""));
			}
		}
		else if (startsWith(line, "Content-Type"))
			continue;
		else if (line == boundary + "\r" || line == boundary + "--\r")
		{
			if (!content.empty())
			{
				if (!filename.empty())
				{
					filename.insert(0, _rqst->getLocation()->getRootPath());
					std::cout << "File: " << filename << '\n';
					std::ofstream ofs(filename.c_str());
					if (not ofs.is_open())
					{
						Logger::Error("Response::UploadMultipart(): failed to open file '%s'", filename.c_str());
						_code = std::make_pair(500, "Internal Server Error");
						return ;
					}
					else
						Logger::Info("Response::UploadMultipart(): file '%s' created", filename.c_str());
					content.erase(0, 2);
					content.erase(content.size() - 2, 2);
					ofs << content;
					ofs.close();
				}
				filename.clear();
				content.clear();
			}
			std::cout << line << std::endl;
		}
		else
			content += line + '\n';
	}
	_code = std::make_pair(200, "OK");
	_body = "Upload succeeded";
}

void	Response::doPOST(void)
{
	Logger::Info("doPOST() entered");
	Request::headers_t	headers = _rqst->getHeaders();
	Request::headers_t::const_iterator	type = headers.find("Content-Type");

	if (type == headers.end())
	{
		_code = std::make_pair(501, "Not Implemented Content-Type");
		return ;
	}
	if (startsWith(type->second, "multipart/form-data"))
	{
		Logger::Info("is multiform()");
		UploadMultipart();
		return ;
	}
	else if (_rqst->getLocation()->isCGIActive() && type->second == "application/x-www-form-urlencoded")
	{
		Logger::Info("is URL encoded");
		phpCgiPost();
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

const std::string &		Response::getBody(void) const
{
	return _body;
}

#define BUFFSIZE_RES 8192
/*
	1) Essai d'ouvrir le fichier : return false si fail (file not found)
	2) Remplit le _response body avec le contenu du fichier, et set le bon Content-Lenght
*/
bool	Response::tryFile(void)
{
	if (_readData.buffer == NULL)
	{
		Logger::Info("GET - Opening %s", _rqst->getTargetPath().c_str());
		_readData.file.open(_rqst->getTargetPath().c_str());
		if (_readData.file.is_open() == false)
			return false;

		// get length of _readData.file:
		_readData.file.seekg(0, _readData.file.end);
		if (not _readData.file.good())
			return false; // TODO: error : _readData.file is directory
		int length = _readData.file.tellg();
		_headers["Content-Length"] = IntToStr(length);
		_readData.file.seekg(0, _readData.file.beg);

		_readData.buffer = new char [BUFFSIZE_RES];
		bzero(_readData.buffer, BUFFSIZE_RES);

		_readData.file.read(_readData.buffer, BUFFSIZE_RES);
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
		// Logger::Info("Respond - Read Data");
		bzero(_readData.buffer, BUFFSIZE_RES);
		_readData.file.read(_readData.buffer, BUFFSIZE_RES);
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
	_cgi.addVarToEnv("REDIRECT_STATUS=true");
	_cgi.addVarToEnv("GATEWAY_INTERFACE=CGI/1.1");
	_cgi.addVarToEnv("PATH_INFO=/"); // ou si myscript.php/this/is/pathinfo?query
	_cgi.addVarToEnv("QUERY_STRING=" + _rqst->getUri().query);
	_cgi.addVarToEnv("CONTENT_LENGTH=" + _rqst->getContentLength());
	_cgi.addVarToEnv("CONTENT_TYPE=" + _rqst->getValForHdr("Content-Type"));
//	_cgi.addVarToEnv("REMOTE_ADDR=" + inet_ntoa(client->addr())); //IP
	_cgi.addVarToEnv("REQUEST_METHOD=" + _rqst->getMethod());
	_cgi.addVarToEnv("SCRIPT_FILENAME=" + _rqst->getLocation()->getRootPath() + _rqst->getUri().path);
	_cgi.addVarToEnv("SCRIPT_NAME=" + _rqst->getUri().path);
	_cgi.addVarToEnv("SERVER_NAME=" + _rqst->getConfig()->getServerNames()[0]);
	_cgi.addVarToEnv("SERVER_PORT=" + IntToStr(_rqst->getConfig()->getPort()));
	_cgi.addVarToEnv("SERVER_PROTOCOL=HTTP/1.1");
	_cgi.addVarToEnv("SERVER_SOFTWARE=WebServ");
	for (Request::headers_t::const_iterator cit = _rqst->getHeaders().begin(); cit != _rqst->getHeaders().end(); ++cit)
	{
		_cgi.addVarToEnv(cit->first + "=" + cit->second);
	}
}

/* CGI GET quand par exemple html fomr pour submit une image qu'on veut afficher
dans le navigateur */
void		Response::phpCgiGet(void)
{
//	Logger::Error("Response GET here targetPath= %s", _rqst->getTargetPath().c_str());
	setCgiEnv();
	_cgi.initFileOut();
	if (_cgi.launch() == -1)
	{
		_code = std::make_pair(500, "Internal Server Error");
		return ;
	}
	if (readFromCgi() == -1)
		_code = std::make_pair(500, "Internal Server Error");
	else
		_code = std::make_pair(200, "OK");
}

/* Remplit le _body depuis les datas read from le pipe du CGI. */
int		Response::readFromCgi(void)
{
	int	fd = _cgi.getOutputFile();
	if (lseek(fd, 0, SEEK_SET) == -1)
		Logger::Error("Response::readFromCgi() lseek() failed");
	char buf[BUFFSIZE] = {};
	ssize_t	nbread = 0;
	nbread = read(fd, buf, BUFFSIZE - 1);
	if (nbread == -1)
	{
		Logger::Error("Response::phpCgiGet() read() failed");
		return -1;
	}
	buf[nbread] = 0;
//	_body += buf;
	std::string		raw(buf, nbread);
	std::string		content = "Content-type:";
	if (raw.find(content) == 0)
	{
		_headers["Content-Type"] = raw.substr(content.size(), raw.find_first_of("\r\n\r\n") - content.size());
	}
	_body.assign(findCRLF(raw.begin(), raw.end()) + 4, raw.end());
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
//	_cgi.CloseFiles();
	return (nbread);
}

void		Response::doDirectoryListening(void)
{
	_body = GenerateHtmlDirectory(_rqst->getTargetPath());
	_code = std::make_pair(200, "OK");
	if (_body.empty())
		_code = std::make_pair(404, "Not Found");
	else
		_headers["Content-Length"] = IntToStr(_body.size());
}

void		Response::doGET(void)
{
	if (_rqst->targetIsDir() && _rqst->getLocation()->isDirList() == true)
	{
		doDirectoryListening();
		return ;
	}
	else if (_rqst->targetIsFile())
	{
		if (tryFile())
			_code = std::make_pair(200, "OK");
		else
			_code = std::make_pair(404, "Not Found");
		return ;
	}
	else if (_rqst->getLocation()->isCGIActive()) // targetIsCgi()
	{
		// NOTE: Add _code = std::make_pair(200, "OK");
		phpCgiGet();
	}
	else
		_code = std::make_pair(404, "Not Found");
}

bool	Response::checkHost(void) const
{
	if (_rqst->getHost() == "UNDEFINED")
		return false;
	return true;
}

bool	Response::checkMethod(void) const
{
	return _rqst->getLocation()->methodIsAllowed(_rqst->getMethod());
}

