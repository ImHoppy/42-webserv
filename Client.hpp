#pragma once

#include "Server.hpp"
#include "Request.hpp"

#include <iostream>
#include <deque>

class Client
{
	int		_csock; // client socket, the one returned by accept() calls
	int		_lsock; // listen socket on which the csock was accepted
	Server*	_servby; // ptr to the Server which had accepted this client
	std::deque<Request>		_pendingRqst; // deque of requests from this clientwhich havn't been respond yet
	public:
		Client(void) {};
		~Client(void) {};
		Client(const Client& src);
		Client&		operator=(const Client& src);
		Client(int csock, int lsock, Server* server);
		void	addRequest(const std::string& raw_rqst);
		void	addRequest(const Request& rqst);

}; // end class Client

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
