#pragma once

#ifndef CONSTRUC
# define CONSTRUC
#endif

#include "Server.hpp"
#include "Request.hpp"
#include "Base.hpp"

#include <iostream>
#include <deque>

#ifndef BUFFSIZE
# define BUFFSIZE 3000
#endif

/*
Une request est associee a un client_fd, qui est associe a un listen_socket,
qui est associe a un Server, et c'est donc ce Server qui va process la request
avec une des ses configs, choisit parmis celles-ci en fonction du Host header
*/

/*	
	Le "createur" d'un Client est responsable de fermer ses socket, et de 
	realease totue memoire allouee (entre autre si le ptr de Server est alloue)
*/

typedef int socket_t;
class Server;

class Client : public Base
{
	socket_t		_csock; // client socket, the one returned by accept() calls
	Server*			_myServer;
	std::deque<Request>		_pendingRqst; // deque of requests from this clientwhich havn't been respond yet
	public:
		Client(void);
		~Client(void);
		Client(const Client& src);
		Client&		operator=(const Client& src);
		Client(socket_t csock, Server* serv);
		int		getSocket(void) const;
		void	addRequest(std::string raw_rqst);
		void	addRequest(const Request& rqst);
		void	popOutRequest(void);
		Request*	getFirstRequest(void);
		const std::deque<Request>&	getPendingRequests(void) const;
		int		recvRequest(void);
		Server*		getServer(void);

		std::string const & getType() const;

}; // end class Client

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
	Request	rqst(raw_rqst);
	this->_pendingRqst.push_back(rqst);
}

/* Add a new Request to the pending queue of request to be respond. */
void	Client::addRequest(const Request& rqst)
{
	this->_pendingRqst.push_back(rqst);
}

void	Client::popOutRequest(void)
{
	this->_pendingRqst.pop_front();
}

Request*	Client::getFirstRequest(void)
{
	if (_pendingRqst.empty())
		return NULL;
	return &(this->_pendingRqst.front());
}

const std::deque<Request>&	Client::getPendingRequests(void) const
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
	std::cout << "POLLIN on client " << _csock << std::endl;
	char buf[BUFFSIZE];
	memset(&buf, 0, sizeof(buf));
	ssize_t bytes = recv(_csock, buf, BUFFSIZE, 0);
	if (bytes < 0)
		throw std::runtime_error("recv failed");
	else if (bytes == 0)
	{
		std::cout << "EOF received from client " << _csock << std::endl;
		return (0);
	}
	else
	{
		buf[bytes] = 0;
		std::cout << bytes << "bytes reveived from client " << _csock << ": \n";
		Request		new_rqst(buf);
		_pendingRqst.push_back(new_rqst);
		std::cout << new_rqst << std::endl;
		return (bytes);
	}
}

Server*		Client::getServer(void)
{
	return _myServer;
}

/* MARCHE PS CAUSE DOUBLE INCLUSION
//TODO: pourauoi pas faire une nested class ou j'en sais rien. Ou alors une structure
pour epoll ptr avec juste deux obj, Client et Server.
void	Client::closeConnection(void)
{
	_myServer->removeClient(this);
}

void	Client::askForResponse(void)
{
	if (_pendingRqst.empty())
		return ;
	_myServer->respond(this);
}
*/


std::string const & Client::getType() const { return _type; }
