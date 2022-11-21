#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <sys/socket.h>
# include <cstring>
# include <netinet/in.h>
# include <limits>
# include <stdlib.h>
# include <sys/epoll.h>
# include <stdio.h> // perror
# include <ctime> //time
# include <iomanip>
# include <unistd.h>
# include <vector>
# include <set>
# include <errno.h>
# include "Base.hpp"
# include "Response.hpp"
# include "Client.hpp"
# include "Logger.hpp"
# include "ServerConfig.hpp"
# include "Request.hpp"

typedef int socket_t;

class Server : public Base
{
	private:
		typedef std::set<Client*>	set_client;
		socket_t					_socket;
		std::vector<ServerConfig>	_configs;
		std::set<Client*>			_clients;
		int							_epollInstance;
		ServerConfig*			getConfigForRequest(Request* rqst);
	public:
		/* Coplien */
		Server(void);
		~Server(void);
		Server(const Server & other);
		Server &	operator=(const Server & other);
		/* Getteurs */
		std::string const &						getType(void) const;
		socket_t								getSocket() const;
		std::vector<ServerConfig> const &		getConf(void) const;
		/* Init fcts */
		void	addConfig(ServerConfig const & config);
		void	setEpollInstance(int inst);
		int		InitServer(void);
		/* HTTP fcts  */
		void	removeClient(Client* client);
		void	respond(Client* client);
		socket_t	 AcceptNewClient(void);
		/* Logs */
		bool	isSameHostPort(int32_t host, int16_t port) const;
};

/* CGI:
	execve php-cgi filepath (ATTENTION need export variable REDIRECT_STATUS=true)
	balise 'action': permet de preciser quel programme doit etre execve 
	ex:		<form action="/cgi-bin/test.cgi" method="get"> 
	Attention: commencer action par "/" permet au navigateur de remplacer le referer
	avec
	attention, le script doit avoir les droit d'exec pour tous
	pour passer la query string (les "arguments/donnes du form) au program: faut set
	une variable d'environnement called QUERY_STRING avec elle (body d'une POST request)
	0) getConfFromUrl a changer: check dabord si fini par .php
	1) append cgi_path + nom du script envoye dans "action" form.
	2)
	3) set les environnements variables (std::vector add les bonnes)
	X) execve la cgi_cmd avec la string(cgi_path+action val) comme 1st arg, et le
	vector.data comme environnement
	X) Recevoir la sortie dans le main: dup2 ou pipe stdout ?
*/

#endif
