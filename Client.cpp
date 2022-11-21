#include "Client.hpp"

/* Default Constructor */
Client::Client(void) : Base("Client"), _csock(-1), _myServer(), _pendingRqst()
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
	for (std::deque<Request*>::iterator it = _pendingRqst.begin(); it != _pendingRqst.end(); ++it)
		delete *it;
	if (_csock >= 0)
		close(_csock);
}

/* Copy Constructor */
Client::Client(const Client& src) :
	Base("Client"),
	_csock(src._csock),
	_myServer(src._myServer),
	_pendingRqst(src._pendingRqst)
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
	_pendingRqst()
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
	_pendingRqst = src._pendingRqst;
	#ifdef CONSTRUC
	std::cerr << "Client Assignement operator" << std::endl;
	#endif
	return *this;
}

/* Add a new Request to the pending queue of requests to be respond. */
void	Client::addRequest(std::string raw_rqst)
{
	Request*	rqst = new Request(raw_rqst);
	this->_pendingRqst.push_back(rqst);
}

/* Add a new Request to the pending queue of request to be respond. */
void	Client::addRequest(Request* rqst)
{
	this->_pendingRqst.push_back(rqst);
}

void	Client::popOutRequest(void)
{
	if (_pendingRqst.empty())
		return ;
	delete *(_pendingRqst.begin());
	this->_pendingRqst.pop_front();
}

Request*	Client::getFirstRequest(void)
{
	if (_pendingRqst.empty())
		return NULL;
	return this->_pendingRqst.front();
}

const std::deque<Request*>&	Client::getPendingRequests(void) const
{
	return this->_pendingRqst;
}

int		Client::getSocket(void) const
{
	return _csock;
}

/* If bytes rcved is 0, closes the connection. Else,
create a Request object with the buf received, and add it int its queued requests. */
int		Client::recvRequest(void)
{
	char buf[BUFFSIZE];
	memset(&buf, 0, sizeof(buf));
	ssize_t bytes = recv(_csock, buf, BUFFSIZE, 0);
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
		_pendingRqst.push_back(new_rqst);
		return (bytes);
	}
}

Server*		Client::getServer(void)
{
	return _myServer;
}

std::string const & Client::getType() const { return _type; }

