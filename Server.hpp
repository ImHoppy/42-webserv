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

#include <errno.h>
typedef int socket_t;

typedef struct s_polldata
{
	Server*	server;
	Client*	client;
} t_polldata;

class Client;

class Server {
	socket_t					_socket;
	std::vector<ServerConfig>	_configs;
	std::set<Client*>			_clients;
	int							_epollInstance;
	typedef std::set<Client*>	set_client;
	public:
	
	/* Default Constructor */
	Server(): _socket(-1), _configs(), _clients(), _epollInstance(-1) {};

	void addConfig(ServerConfig const & config) {
		_configs.push_back(config);
	};
	
	void	setEpollInstance(int inst)
	{
		_epollInstance = inst;
	}

	/* Create, bind, and set in listen state its _socket. */
	int		InitServer()
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
	};

	/* Copy Constructor */
	Server(Server const & other) {
		*this = other;
	};

	private:
	Server & operator=(Server const & other) {
		if (this != &other) {
			_socket = other._socket;
			_configs = other._configs;
			_clients = other._clients;
		}
		return *this;
	};

	/* Destructor */
	public:
	~Server() {
		for (set_client::iterator	it = _clients.begin(); it != _clients.end(); ++it)
		{
			delete *it;
		}
	};

	/* Create a new client socket with accept, and create a new Client instance with it.
	Then add it with its pair client socket to the _clients set. */
	socket_t	 AcceptNewClient(void)
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
			std::cout << "Here\n";
			return (-1);
		}
		Client*	new_client = new Client(client_socket, this);
		t_polldata* data = new t_polldata;
		data->server = this;
		data->client = new_client;
		event.data.ptr = reinterpret_cast<void *>(data);
		event.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
			throw std::runtime_error("epoll_ctl failed");
		}
		_clients.insert(new_client);
		displayTime();
		std::cout << "Server #" << ": accepted new client " << client_socket << std::endl;
		return (client_socket);
	}

	void	displayTime(void) const
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

	socket_t getSocket() const {
		return _socket;
	}

	std::vector<ServerConfig> const &		getConf(void) const {
		return _configs;
	}

	/* Terminates connection with the 'client': deletes it frome the epoll fds,
	close the connected socket, delete its memory and removes it from the set of Clients.
	*/
	void	removeClient(Client* client)
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

	void	respond(Client* client)
	{
		Request*	rqst = client->getFirstRequest();
		if (rqst && rqst->method() == "GET")
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

};
