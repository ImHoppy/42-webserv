#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "Server.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "Base.hpp"

# include <iostream>
# include <deque>
# include <unistd.h>

# ifndef BUFFSIZE
#  define BUFFSIZE 3000
# endif

#define TIMEOUT 60000

#include "Time.hpp"

/*	
	Le "createur" d'un Client est responsable de fermer ses socket, et de 
	realease totue memoire allouee (entre autre si le ptr de Server est alloue)
*/

typedef int socket_t;
class Server;
class Response;

class Client : public Base
{
	private:
		socket_t		_csock; // client socket, the one returned by accept() calls
		Server*			_myServer;
		Request*		_Rqst;
		Response*		_Resp;
		std::ofstream	_file;
		bool			_error;
		Timeout			_keepAlive;

		void			createNewRequest(char * buf, size_t & start_buf, ssize_t & bytes);

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
		void	popOutResponse(void);
		Request*	getRequest(void);
		int			recvRequest(void);
		Server*		getServer(void);
		void		setResponse(Response* resp);
		Response*	getResponse(void) const;
		bool		hasError(void) const;

		void		removeTmpFile(void);

		bool		hasTimeout(void) const;

		std::string const & getType() const;

}; // end class Client
#endif
