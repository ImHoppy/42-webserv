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

#ifndef BUFFSIZE
# define BUFFSIZE 3000
#endif


#include <errno.h>
typedef int socket_t;

class Server {
	socket_t						_socket;
	std::vector<ServerConfig>		_configs;
	std::map<socket_t, Client*>		_clients;
	int								_index;	
	typedef std::map<socket_t, Client*>		map_client;
	public:
	
	/* Default Constructor */
	Server(): _socket(-1), _configs(), _clients(), _index(-1) {};

	void addConfig(ServerConfig const & config) {
		_configs.push_back(config);
	};

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
			_index = other._index;
		}
		return *this;
	};

	/* Destructor */
	public:
	~Server() {
		for (map_client::iterator	it = _clients.begin(); it != _clients.end(); ++it)
		{
			delete it->second;
		}
	};

	/* Create a new client socket with accept, and create a new Client instance with it.
	Then add it with its pair client socket to the _clients map. */
	socket_t	 AcceptNewClient(socket_t & epollInstance)
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
		event.data.fd = client_socket;
		event.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
			throw std::runtime_error("epoll_ctl failed");
		}
		Client*	new_client = new Client(event.data.fd, _socket);
		_clients.insert(std::make_pair(client_socket, new_client));
		displayTime();
		std::cout << "Server #" << _index << ": accepted new client " << client_socket << std::endl;
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

	void	setIndex(int index) {
		_index = index;
	}
	socket_t getSocket() const {
		return _socket;
	}

	std::vector<ServerConfig> const &		getConf(void) const {
		return _configs;
	}

	/* Read (with recv()) from the cient socket 'csocket'. If bytes rcved is 0, closes
	th connection with the client. Else, create a Request object with the buf received,
	and add it the its Client queud requests. */
	int	recvRequest(socket_t csocket, socket_t epollInst)
	{
		std::cout << "POLLIN: serv " << _socket << " client " << csocket << std::endl;
		char buf[BUFFSIZE];
		memset(&buf, 0, sizeof(buf));
		ssize_t bytes = recv(csocket, buf, BUFFSIZE, 0);
		if (bytes < 0)
			throw std::runtime_error("recv failed");
		else if (bytes == 0)
		{
			std::cout << "EOF received from client " << csocket << std::endl;
			removeClient(csocket, epollInst);
			return (0);
		}
		else
		{
			buf[bytes] = 0;
			std::cout << bytes << "bytes reveived from client " << csocket << ": \n";
			map_client::iterator	it = _clients.find(csocket);
			if (it == _clients.end())
				return (0);
			it->second->addRequest(buf);
			return (bytes);
		}
	}

	void	removeClient(socket_t csocket, socket_t epollInst)
	{
			if (epoll_ctl(epollInst, EPOLL_CTL_DEL, csocket, NULL) < 0) {
				throw std::runtime_error("epoll_ctl DEL after EOF failed");
			}
			close(csocket);
			delete _clients[csocket];
			_clients.erase(csocket);
	}

	void	respond(socket_t csocket)
	{
		map_client::iterator	it = _clients.find(csocket);
		if (it == _clients.end()) // si pas trouve, pas normal btw
			return ;
		Client *client = it->second;
		if (client->getPendingRequests().empty()) // si pas de pending request
			return ;
		Request&	rqst = client->getFirstRequest();
		if (rqst.method() == "GET")
		{
			std::cout << "POLLOUT event on client fd " << csocket <<std::endl;
			ssize_t		bytes;
			std::string buf("HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Keep-Alive\r\n\r\nHello, world!");
			bytes = send(csocket, buf.c_str(), buf.size(), 0);
			if (bytes == -1)
			{
				throw std::runtime_error("send failed");
			}
			std::cout << bytes << " send to client " << csocket << std::endl;
			client->popoutRequest();
		}
	}

};
