#pragma once

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <limits>
#include <stdlib.h>
#include <sys/epoll.h>
#include <stdio.h> // perror

#include <errno.h>
typedef int socket_t;

class Server {
	socket_t					_socket;
	std::vector<ServerConfig>	_configs;
	std::vector<socket_t>		_clients;
	public:
	Server(): _socket(-1), _configs(), _clients() {};

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

	Server(Server const & other) {
		*this = other;
	};
	Server & operator=(Server const & other) {
		if (this != &other) {
			_socket = other._socket;
			_configs = other._configs;
			_clients = other._clients;
		}
		return *this;
	};

	~Server() {
		close(_socket);
	};

	void AcceptNewClient(socket_t & epollInstance)
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
			return;
		}
		event.data.fd = client_socket;
		event.events = EPOLLIN | EPOLLOUT;
		if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
			throw std::runtime_error("epoll_ctl failed");
		}
		addClient(event.data.fd);
	}

	void handleRequest() {

	}

	void	addClient(socket_t client) {
		_clients.push_back(client);
	}

	socket_t getSocket() const {
		return _socket;
	}

	std::vector<ServerConfig> const &		getConf(void) const {
		return _configs;
	}

};
