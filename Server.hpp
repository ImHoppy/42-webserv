#pragma once

#include <iostream>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <limits>
#include <stdlib.h>
#include <sys/epoll.h>

#include <errno.h>
typedef int socket_t;

class Server {
	socket_t _socket;
	bool _listen;
	std::vector<ServerConfig>	configs;
	std::vector<socket_t>		clients;
	public:
	Server() {};

	void addConfig(ServerConfig const & config) {
		configs.push_back(config);
	};
	void InitServer() {
		int on = 1;
		struct sockaddr_in servaddr;

		/*************************************************************/
		/*                      Create socket                        */
		/*************************************************************/
		if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
		{
			std::cerr << "error: socket()" << std::endl;
			exit(1);
		}

		/*************************************************************/
		/*       Allow socket descriptor to be reuseable             */
		/*************************************************************/
		if (setsockopt(_socket, SOL_SOCKET,  SO_REUSEADDR | SO_REUSEPORT, &on, sizeof(int)) < 0)
		{
			std::cerr << "error: setsockopt() failed" << std::endl;
			close(_socket);
			exit(1);
		}

		/*************************************************************/
		/*               Fill the sock_addr_in struct                */
		/*************************************************************/
		std::memset((char*)&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_addr.s_addr = configs[0].getHost();
		servaddr.sin_port = htons(configs[0].getPort());

		/*************************************************************/
		/*                       Bind the socket                     */
		/*************************************************************/
		if (bind(_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
		{
			std::cerr << "error: bind() failed" << std::endl;
			exit(1);
		}
		/*************************************************************/
		/*                       Listen the socket                   */
		/*************************************************************/


	};
	Server(Server const & other) {
		*this = other;
	};
	Server & operator=(Server const & other) {
		if (this != &other) {
			_socket = other._socket;
			_listen = other._listen;
			configs = other.configs;
			clients = other.clients;
		}
		return *this;
	};
	~Server() {
		close(_socket);
	};
	void AcceptNewClient(socket_t epollInstance)
	{
		if (isListen() == false) {
			if (listen(_socket, std::numeric_limits<short>::max()/2) < 0)
			{
				throw std::runtime_error("listen() failed");
			}
		}
		// NOTE: else Si accept a besoin d'etre sur un nouveau epoll
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
				return;
			event.data.fd = client_socket;
			event.events = EPOLLIN | EPOLLOUT;
			if (epoll_ctl(epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
				throw std::runtime_error("epoll_ctl failed");
			}
			addClient(event.data.fd);
		}
	}

	void handleRequest() {

	}

	void	addClient(socket_t client) {
		clients.push_back(client);
	}
	void	setListen(bool listen) {
		_listen = listen;
	}
	bool isListen() {
		return _listen;
	}
	socket_t getSocket() const {
		return _socket;
	}
};
