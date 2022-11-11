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
	Server*	_servby; // ptr to the Server which had accepted this client
	std::deque<Request>		_pendingRqst; // deque of requests from this clientwhich havn't been respond yet
	public:
		Client(void);
		~Client(void);
		Client(const Client& src);
		Client&		operator=(const Client& src);
		Client(int csock, int lsock, Server* server);
		void	addRequest(const std::string& raw_rqst);
		void	addRequest(const Request& rqst);

}; // end class Client

/* Default Constructor */
Client::Client(void) : _csock(-1), _lsock(-1), _servby(), _pendingRqst() {}

/* Destructor */
Client::~Client(void) {}

/* Copy Constructor */
Client::Client(const Client& src) :
	_csock(src._csock),
	_lsock(src._lsock),
	_servby(src._servby),
	_pendingRqst(src._pendingRqst) {}

/* Parametric Constructor (with empty pending requests) */
Client::Client(int csock, int lsock, Server* server) :
	_csock(csock),
	_lsock(lsock),
	_servby(server),
	_pendingRqst() {}

/* Assignement operator */
Client&		Client::operator=(const Client& src)
{
	if (this == &src)
		return *this;
	_csock = src._csock;
	_lsock = src._lsock;
	_servby = src._servby;
	_pendingRqst = src._pendingRqst;
	return *this;
}

/* Add a new Request to the pending queue of request to be respond. */
void	Client::addRequest(const std::string& raw_rqst)
{
	_pendingRqst.push_back(Request(raw_rqst));
}

/* Add a new Request to the pending queue of request to be respond. */
void	Client::addRequest(const Request& rqst)
{
	_pendingRqst.push_back(rqst);
}
