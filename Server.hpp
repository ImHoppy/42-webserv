#pragma once

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <limits>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h> // perror
#include "Client.hpp"
#include <ctime> //time
#include <iomanip>
#include <unistd.h>
#include <vector>
#include <set>
#include "Base.hpp"
#include "Response.hpp"

#ifndef CONSTRUC
# define CONSTRUC
#endif

#include <errno.h>
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
		ServerConfig*	getConfigForRequest(Request* rqst);
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
		void	displayTime(void) const;
};
	
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

/* Une response est fct de la requete evidemment, et de la config:
	1) Trouver la bonne config: check le host header pour avoir le server name
	ou l'URI
	2) Bah regarder ce qu'on nous demande dessus
Dans nginx default:
	location / {
		try_files $uri $uri/ =404
	} 
ils expliquent = Firs attempt to serve request as file, then as directory, then
fallback to displaying a 404
*/
void	Server::respond(Client* client)
{
	if (client == NULL) return;
	Request*	rqst = client->getFirstRequest();
	if (rqst == NULL)
		return ;
	ServerConfig*	chosen_conf = getConfigForRequest(rqst);
	std::cout << "_______Chosen config is: \n";
	std::cout << *chosen_conf;
	std::string	response = chosen_conf->respondRequest(*rqst);
	std::cout << "_____RESPONSE FROM CONFIG IS:\n\'" << response << "\'\n____ EnD RESPONSE CONF."<< std::endl;
	std::cout << "\n_______END chosen conf" << std::endl;
	if (rqst != NULL && rqst->getMethod() == "GET")
	{
		socket_t	socket = client->getSocket();
		std::cout << "POLLOUT event on client fd " << socket <<std::endl;
		ssize_t		bytes;
		std::string buf("HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Keep-Alive\r\n\r\nHello, world!");
		bytes = send(socket, buf.c_str(), buf.size(), 0);
		if (bytes == -1)
		{
			throw std::runtime_error("send failed");
		}
		std::cout << bytes << " send to client " << socket << std::endl;
		client->popOutRequest();
	}
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
	int on = 1;
	struct sockaddr_in servaddr;

	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		std::cerr << "error: socket()" << std::endl;
		return (-1);
	}
	if (setsockopt(_socket, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
	{
		std::cerr << "error: setsockopt() failed" << std::endl;
		close(_socket);
		_socket = -1;
		return (-1);
	}
	std::memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = _configs[0].getHost();
	servaddr.sin_port = htons(_configs[0].getPort());
	if (bind(_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		std::cerr << "error: bind() failed" << std::endl;
		return (-1);
	}
	if (listen(_socket, std::numeric_limits<short>::max()/2) < 0)
	{
		perror("Server: Listen failed");
		return (-1);
	}
	return (0);
}

void	Server::displayTime(void) const
{
	std::time_t time = std::time(0);
	std::tm *tm = std::localtime(&time);
	std::cout << std::setfill('0') << "["
	<< std::setw(4) << (tm->tm_year + 1900)
	<< std::setw(2) << (tm->tm_mon + 1)
	<< std::setw(2) << tm->tm_mday
	<< "_"
	<< std::setw(2) << tm->tm_hour
	<< std::setw(2) <<tm->tm_min
	<< std::setw(2) << tm->tm_sec
	<< "] ";
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
	displayTime();
	std::cout << "Server #" << ": accepted new client " << client_socket << std::endl;
	return (client_socket);
}
