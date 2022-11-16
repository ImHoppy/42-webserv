#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include "ServerConfig.hpp"
# include "Server.hpp"

# include <vector>
# include <sys/epoll.h>
# include <unistd.h>
# include <csignal>

#define MAX_EVENTS 100 // NOTE: 4096

void	sig_handler(int sig);

class WebServ
{
	public:
		typedef std::vector<Server*>				vec_servers;

	private:
		std::vector<Server*>				_servers;
		socket_t							_epollInstance;
	
	public:
		static bool									_isRunning;
		// pourquoi socket_t ?? 
		static std::map<socket_t, std::string>		error_status;

		void	addServer(Server *serv);
		void	InitEpoll(void);
		void	StartLoop(void);
		void	EndLoop(void);
		const std::vector<Server*> &				getServers(void) const;
		std::vector<Server*>::iterator		checkIpPort(const ServerConfig& toAdd);

		/* Coplien */
		WebServ(void);
		WebServ(WebServ const & other);
		WebServ & operator=(WebServ const & other);
		~WebServ(void);

};// end class WebServ

//TODO: a deplacer
void	sig_handler(int sig)
{
	(void)sig;
	std::cout << "SIG HANDLER REACHED" << std::endl;
	WebServ::_isRunning = false;
}

#endif
