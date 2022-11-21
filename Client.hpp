#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "Base.hpp"

# include <iostream>
# include <deque>
# include <unistd.h>

# ifndef BUFFSIZE
#  define BUFFSIZE 3000
# endif

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
	std::deque<Request*>		_pendingRqst; // deque of requests from this clientwhich havn't been respond yet
	public:
		Client(void);
		~Client(void);
		Client(const Client& src);
		Client&		operator=(const Client& src);
		Client(socket_t csock, Server* serv);
		int		getSocket(void) const;
		void	addRequest(std::string raw_rqst);
		void	addRequest(Request* rqst);
		void	popOutRequest(void);
		Request*	getFirstRequest(void);
		const std::deque<Request*>&	getPendingRequests(void) const;
		int			recvRequest(void);
		Server*		getServer(void);

		std::string const & getType() const;

}; // end class Client
#endif
