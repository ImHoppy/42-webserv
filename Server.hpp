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
# include "Client.hpp"
# include <ctime> //time
# include <iomanip>
# include <unistd.h>
# include <vector>
# include <set>
# include "Base.hpp"
# include "Response.hpp"
# include <errno.h>
# include "Logger.hpp"

# define CONSTRUC

typedef int socket_t;

typedef struct s_polldata
{
	Server*	server;
	Client*	client;
} t_polldata;

class Client;

class Server : public Base {
	private:
		typedef std::set<Client*>	set_client;
		socket_t					_socket;
		std::vector<ServerConfig>	_configs;
		std::set<Client*>			_clients;
		int							_epollInstance;
		ServerConfig*				getConfigForRequest(Request* rqst);
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

/* Return true if one of the _configs has the same HOST::PORT pairs. */
bool	Server::isSameHostPort(int32_t host, int16_t port) const
{
	for (std::vector<ServerConfig>::const_iterator conf_it = _configs.begin(); conf_it != _configs.end(); ++conf_it)
	{
		if (conf_it->getPort() == port && conf_it->getHost() == host)
			return true;
	}
	return false;
}
	
/* Default Constructor */
Server::Server(void) : Base("Server"), _socket(-1), _configs(),
	_clients(), _epollInstance(-1)
{
	#ifdef CONSTRUC
	std::cerr << "Server Default constructor" << std::endl;
	#endif
}

/* Copy Constructor */
Server::Server(Server const & other) : Base("Server"), _socket(other._socket), 
	_configs(other._configs), _clients(other._clients), 
_epollInstance(other._epollInstance)
{
	#ifdef CONSTRUC
	std::cerr << "Server Copy constructor" << std::endl;
	#endif
}

/* Assignement operator (should be private) */
Server &	Server::operator=(Server const & other)
{
	#ifdef CONSTRUC
	std::cerr << "Server Assignement operator" << std::endl;
	#endif
	if (this != &other) {
		_socket = other._socket;
		_configs = other._configs;
		_clients = other._clients;
	}
	return *this;
}

/* Destructor */
Server::~Server(void)
{
	#ifdef CONSTRUC
	std::cerr << "Server Destructor" << std::endl;
	#endif
	for (set_client::iterator	it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete *it;
	}
}

std::vector<ServerConfig> const &	Server::getConf(void) const { return _configs; }
socket_t	Server::getSocket(void) const { return _socket; }
std::string const &	Server::getType(void) const { return _type; }

void	Server::addConfig(ServerConfig const & config)
{
	_configs.push_back(config);
}

void	Server::setEpollInstance(int inst)
{
	_epollInstance = inst;
}

/* Cherche parmis les configs de ce server (en gros parmis ses virtuals servers)
lequel a le meme server_name que le Host header de la requete. 
Return:
	1) Si aucun Host header n'est present, on return le VServer par default (en
	gros le premier). Celui-ci se chargera de respond 400 bad request.
	2) Return le bon VServer qui a un server_names == host header value.
	3) Si la requete a bien un Host header, mais qu'aucun VServer name ne 
	correpond a celui-ci, le default VServer se chargera de servir la rqst.
*/
ServerConfig*	Server::getConfigForRequest(Request* rqst)
{
	std::string		host_header = rqst->getHost();
	if (host_header == "UNDEFINED")
		return &_configs[0];
	size_t		dotPort = host_header.find(':');
	if (dotPort != std::string::npos)
		host_header.erase(dotPort);
	for (std::vector<ServerConfig>::iterator conf_it = _configs.begin(); conf_it != _configs.end(); ++conf_it)
	{
		std::vector<std::string>	names = conf_it->getServerNames();
		for (std::vector<std::string>::iterator names_it = names.begin(); names_it != names.end(); ++names_it)
		{
			if (*names_it == host_header)
				return &(*conf_it);
		}
	}
	return &_configs[0];
}

/* Terminates connection with the 'client': deletes it frome the epoll fds,
close the connected socket, delete its memory and removes it from the set 
of Clients.*/
void	Server::removeClient(Client* client)
{
	socket_t socket = client->getSocket();
	if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, socket, NULL) < 0) {
		throw std::runtime_error("epoll_ctl DEL after EOF failed");
	}
	close(socket);
	set_client::iterator	it = _clients.find(client);
	if (it != _clients.end())
	{
		delete *it;
		_clients.erase(it);
	}
}

/* Create, bind, and set in listen state its _socket. */
int		Server::InitServer(void)
{
	struct sockaddr_in servaddr;

	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		Logger::Error("Server: socket() failed: %s", strerror(errno));
		return (-1);
	}
	//TODO: launch 2 webserv avec les meme host:port marche a cause de REUSE etc
	int on = 1;
	if (setsockopt(_socket, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
	{
		Logger::Error("Server: setsockopt() failed: %s", strerror(errno));
		close(_socket);
		_socket = -1;
		return (-1);
	}
	std::memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(_configs[0].getHost());
	servaddr.sin_port = htons(_configs[0].getPort());
	if (bind(_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		Logger::Error("Server: bind() failed: %s", strerror(errno));
		return (-1);
	}
	if (listen(_socket, std::numeric_limits<short>::max()/2) < 0)
	{
		perror("Server: Listen failed");
		return (-1);
	}
	return (0);
}

/* Create a new client socket with accept, and create a new Client instance with it.
Then add it with its pair client socket to the _clients set. */
socket_t	 Server::AcceptNewClient(void)
{
	struct epoll_event event;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	socket_t client_socket = accept(_socket, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_socket < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
	{
		//TODO: pb quand HOST different de 0.0.0.0 AH mais en fait c'est la VM qui est sur 127.0.1.1 je crois à tester
		throw std::runtime_error("accept() failed");
	}
	else if (client_socket < 0)
	{
		perror("accept() failed");
		return (-1);
	}
	Client * client = new Client(client_socket, this);
	event.data.ptr = client; // addr de this Server
	event.events = EPOLLIN | EPOLLOUT;
	if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
		throw std::runtime_error("epoll_ctl failed");
	}
	_clients.insert(client);
	Logger::Info("Server %d accepted new client on fd %d", _socket, client_socket);
	return (client_socket);
}

/*	1) Find la bonne config grace au host header
	2) Find la bonne location grace a l'URL
	3) Cree une Resposne avec ces donees
	4) Send la Response
*/
void	Server::respond(Client* client)
{
	if (client == NULL) return;
	Request*	rqst = client->getFirstRequest();
	if (rqst == NULL)
		return ;
	std::cerr << " REQUEST IS:\n" << *rqst << "_____ END REQUEST" << std::endl;
	std::cerr << "Request body:\n" << rqst->getBody() << std::endl;
	std::cerr << "RAW RQST:\n" << rqst->getRawRequest() << std::endl;
	ServerConfig*	chosen_conf = getConfigForRequest(rqst);
//	std::cerr << "____ CHOSEN CONFIG:\n" << *chosen_conf << "______END CHOSEN CONFIG" << std::endl;
	LocationConfig*	chosen_loc = chosen_conf->getLocationFromUrl(rqst->getUri().path);
	std::cerr << "____ CHOSEN LOCATION:\n" << *chosen_loc << "______END CHOSEN LOC" << std::endl;
	Response	rep(chosen_conf, chosen_loc, rqst);
	std::cerr << "____ RESPONSE:\n" << rep << "______END RESPONSE" << std::endl;
	socket_t	socket = client->getSocket();
	ssize_t		bytes;
	bytes = send(socket, rep.getResponse().c_str(), rep.getResponse().size(), 0);
	if (bytes == -1)
	{
		throw std::runtime_error("send failed");
	}
//	Logger::Info("Request for '%s' respond by %s", rqst->getRequestLine().c_str(), chosen_conf->getServerNames()[0].c_str());
	client->popOutRequest();
	//TODO: if _pendingRqst du client is empty, epollCTL MODify events to POLLIN only,
	// et pas oublier de remettre POLLOUT a reception de la premiere request
}

/* CGI:
	execve php-cgi filepath (ATTENTION need export variable REDIRECT_STATUS=true)
	balise 'action': permet de preciser quel programme doit etre execve 
	ex:		<form action="/cgi-bin/test.cgi" method="get"> 
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
