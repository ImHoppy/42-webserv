#pragma once

#include "Server.hpp"
#include "Request.hpp"

#include <iostream>
#include <deque>

/*
Une request est associee a un client_fd, qui est associe a un listen_socket,
qui est associe a un Server, et c'est donc ce Server qui va process la request
avec une des ses configs, choisit parmis celles-ci en fonction du Host header
*/

/*	
	Le "createur" d'un Client est responsable de fermer ses socket, et de 
	realease totue memoire allouee (entre autre si le ptr de Server est alloue)
*/

class Client
{
	int		_csock; // client socket, the one returned by accept() calls
	int		_lsock; // listen socket on which the csock was accepted
	std::deque<Request>		_pendingRqst; // deque of requests from this clientwhich havn't been respond yet
	public:
		Client(void);
		~Client(void);
		Client(const Client& src);
		Client&		operator=(const Client& src);
		Client(int csock, int lsock);
		int		getClientSocket(void) const;
		int		getListenSocket(void) const;
		void	addRequest(std::string raw_rqst);
		void	addRequest(const Request& rqst);
		void	popoutRequest(void);
		Request&	getFirstRequest(void);
		const std::deque<Request>&	getPendingRequests(void) const;
}; // end class Client

/* Default Constructor */
Client::Client(void) : _csock(-1), _lsock(-1), _pendingRqst() {}

/* Destructor */
Client::~Client(void) {}

/* Copy Constructor */
Client::Client(const Client& src) :
	_csock(src._csock),
	_lsock(src._lsock),
	_pendingRqst(src._pendingRqst) {}

/* Parametric Constructor (with empty pending requests) */
Client::Client(int csock, int lsock) :
	_csock(csock),
	_lsock(lsock),
	_pendingRqst() {}

/* Assignement operator */
Client&		Client::operator=(const Client& src)
{
	if (this == &src)
		return *this;
	_csock = src._csock;
	_lsock = src._lsock;
	_pendingRqst = src._pendingRqst;
	return *this;
}

/* Add a new Request to the pending queue of request to be respond. */
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

void	Client::popoutRequest(void)
{
	this->_pendingRqst.pop_front();
}

Request&	Client::getFirstRequest(void)
{
	return this->_pendingRqst.front();
}

const std::deque<Request>&	Client::getPendingRequests(void) const
{
	return this->_pendingRqst;
}

int		Client::getClientSocket(void) const
{
	return _csock;
}

int		Client::getListenSocket(void) const
{
	return _lsock;
}
