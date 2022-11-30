#include "Client.hpp"

/* Default Constructor */
Client::Client(void) : Base("Client"),
	_csock(-1),
	_myServer(),
	_Rqst(),
	_Resp(),
	_error(false)
{
	#ifdef CONSTRUC
	std::cerr << "Client Default constructor" << std::endl;
	#endif
}

/* Destructor */
Client::~Client(void)
{
	#ifdef CONSTRUC
	std::cerr << "Client Destructor" << std::endl;
	#endif
	Logger::Warning("Client destructor");
	if (_Rqst != NULL)
		delete _Rqst;
	if (_Resp != NULL)
		delete _Resp;
	if (_csock >= 0)
		close(_csock);
	if (_file.is_open())
		_file.close();
}

/* Copy Constructor */
Client::Client(const Client& src) :
	Base("Client"),
	_csock(src._csock),
	_myServer(src._myServer),
	_Rqst(src._Rqst),
	_Resp(),
	_error(false)
{
	#ifdef CONSTRUC
	std::cerr << "Client Copy constructor" << std::endl;
	#endif
}

/* Parametric Constructor (with empty pending requests) */
Client::Client(socket_t csock, Server* serv) :
	Base("Client"),
	_csock(csock),
	_myServer(serv),
	_Rqst(),
	_Resp(),
	_error(false)
{
	#ifdef CONSTRUC
	std::cerr << "Client Parametric constructor" << std::endl;
	#endif
}

/* Assignement operator */
Client&		Client::operator=(const Client& src)
{
	if (this == &src)
		return *this;
	_csock = src._csock;
	_myServer = src._myServer;
	_Rqst = src._Rqst;
	#ifdef CONSTRUC
	std::cerr << "Client Assignement operator" << std::endl;
	#endif
	return *this;
}

void	Client::popOutRequest(void)
{
	delete _Rqst;
	_Rqst = NULL;
}


void	Client::popOutResponse(void)
{
	delete _Resp;
	_Resp = NULL;
}

Request*	Client::getRequest(void)
{
	return this->_Rqst;
}

int		Client::getSocket(void) const
{
	return _csock;
}
enum {
	RECV_OK = -1,
	RECV_EOF = -2
};

void	Client::createNewRequest(char * buf, size_t & start_buf, ssize_t & bytes)
{
	std::cout << "socket = " << _csock << std::endl;
	Logger::Info("Client: new Request received from client %d", _csock);
	_Rqst = new Request(buf, start_buf, bytes);

	ServerConfig* chosen_conf = _myServer->getConfigForRequest(_Rqst);
	if (chosen_conf == NULL)
	{
		Logger::Error("Request: - CONFIG NULL");
		return ;
	}

	LocationConfig* chosen_loc = chosen_conf->getLocationFromUrl(_Rqst->getUri().path);
	if (chosen_loc == NULL)
	{
		Logger::Error("Request: - LOCATION NULL");
		return ;
	}
	_Rqst->setConfig(chosen_conf);
	_Rqst->setLocation(chosen_loc);
	_Rqst->setTargetPath();
	if (_Rqst->getMethod() == "POST")
	{
		Request::headers_t::const_iterator		content_length = _Rqst->getHeaders().find("Content-Length");
		if (content_length != _Rqst->getHeaders().end())
		{
			if (StrToInt(content_length->second) > _Rqst->getConfig()->getMaxBodySize())
			{
				Logger::Error("Request: - POST body too big");
				_error = true;
				_myServer->readyToRead(this);
				return ;
			}
		}

		std::string fileName = generateFileName(time(NULL) + _csock);
		_Rqst->setUploadFile(fileName);
		_file.open(fileName.c_str());
		if (not _file.is_open())
			throw std::runtime_error("file for POST cant be open");
	}
}

/* If bytes rcved is 0, closes the connection. Else,
create a Request object with the buf received, and add it int its queued requests. */
int		Client::recvRequest(void)
{
	char buf[BUFFSIZE];
	memset(buf, 0, sizeof(buf));
	ssize_t bytes = recv(_csock, buf, BUFFSIZE - 1, 0);
	if (bytes < 0)
		throw std::runtime_error("recv failed");
	else if (bytes == 0)
	{
		Logger::Info("Client: EOF received from client %d", _csock);
		return (0);
	}
	else
	{
		buf[bytes] = 0;
		if (_Rqst == NULL)
		{
			size_t start_buf = 0;
			createNewRequest(buf, start_buf, bytes);
			if (_file.is_open())
				_file.write(buf+start_buf, bytes);
		}
		else
		{
			Request::headers_t hed = _Rqst->getHeaders();
//			_Rqst->appendToBody(buf);
			if (_file.is_open())
				_file.write(buf, bytes);
			Logger::Info("Client: got new Chunk %d/%s", (long)_file.tellp(), hed["Content-Length"].c_str());
		}
 		Request::headers_t::const_iterator		content_length = _Rqst->getHeaders().find("Content-Length");
		if (content_length != _Rqst->getHeaders().end())
		{

			size_t	len = StrToInt(content_length->second);
			if (len > (unsigned long)_file.tellp()) // Body pas full recu
			{
				return (RECV_OK);
			}
		}
		_myServer->readyToRead(this);
		if (_file.is_open())
			_file.close();
		Logger::Error("Bytes returned = %d", bytes);
		return (bytes);
	}
}

// TODO: Unused
void		Client::removeTmpFile(void)
{
	if (_file.is_open())
		_file.close();
}

Server*		Client::getServer(void)
{
	return _myServer;
}

Response*	Client::getResponse(void) const
{
	return _Resp;
}
void	Client::setResponse(Response* resp)
{
	_Resp = resp;
}

bool	Client::hasError(void) const
{
	return _error;
}

std::string const & Client::getType() const { return _type; }

