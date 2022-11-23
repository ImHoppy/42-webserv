#include "Client.hpp"

/* Default Constructor */
Client::Client(void) : Base("Client"),
	_csock(-1),
	_myServer(),
	_Rqst(),
	_Resp()
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
}

/* Copy Constructor */
Client::Client(const Client& src) :
	Base("Client"),
	_csock(src._csock),
	_myServer(src._myServer),
	_Rqst(src._Rqst),
	_Resp()
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
	_Resp()
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

/* If bytes rcved is 0, closes the connection. Else,
create a Request object with the buf received, and add it int its queued requests. */
int		Client::recvRequest(void)
{
	if (_Rqst != NULL)
	{
		char buf[BUFFSIZE];
		memset(&buf, 0, sizeof(buf));
		ssize_t bytes = recv(_csock, buf, BUFFSIZE - 1, 0);
		if (bytes < 0)
			throw std::runtime_error("recv failed");
		else if (bytes == 0)
		{
//			Logger::Info("Client HERE: EOF received from client %d", _csock);
			return (1);
		}
		else
		{
			buf[bytes] = 0;
			Logger::Info("Client HERE: new Request received from client %d", _csock);
			std::cout << "START CHUNK ________" << buf << "END CHUNK_______" << std::endl;
			return (bytes);
		}
	}
	char buf[BUFFSIZE];
	memset(&buf, 0, sizeof(buf));
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
		Logger::Info("Client: new Request received from client %d", _csock);
		Request*		new_rqst = new Request(buf);
		_Rqst = new_rqst;
		// TODO: Chunk request
/* 		Request::const_iterator		content_length = new_rqst->getHeaders().find("Content-Length");
		if (len != new_rqst->getHeaders().end())
		{
			int		clen = StrToInt(content_length->second);
			if (clen > new_rqst->getBody().size()) // Body pas full recu
				return (-2); // signifie need read encore PAS pollout
		} */
		return (bytes);
	}
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

std::string const & Client::getType() const { return _type; }


