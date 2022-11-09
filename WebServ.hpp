#pragma once

#include "ServerConfig.hpp"
#include "Server.hpp"

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>

class WebServ {
	private:
		std::vector<Server>	_servers;
		socket_t			_epollInstance;
		bool				_isRunning;
	public:
		typedef std::vector<Server> vec_servers;
		static std::map<socket_t, std::string>	error_status;
	void CreateServer() {
		// Server *serv = new Server();
		// _servers[serv->getSocket()] = serv;
	};

	WebServ(): _epollInstance(0), _isRunning(false) {
	};

	WebServ(WebServ const & other) {
		*this = other;
	};

	WebServ & operator=(WebServ const & other) {
		if (this != &other) {
			_servers = other._servers;
			_epollInstance = other._epollInstance;
			_isRunning = other._isRunning;
		}
		return *this;
	};

	~WebServ() {
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			const socket_t socket = it->getSocket();

			if (_epollInstance > 0)
			{ 
				if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, socket, NULL) < 0) {
					throw std::runtime_error("epoll_ctl del failed");
				}
			}
			if (socket > 0)
				close(socket);
		}
		if (_epollInstance > 0)
			close(_epollInstance);
	};

	void addServer(Server &serv) {
		_servers.push_back(serv);
	};

	void EndLoop() {
		_isRunning = false;
	};
	void InitEpoll() {
		if (_servers.size() == 0)
			throw std::runtime_error("No servers to listen to");
		_epollInstance = epoll_create(_servers.size());
		if (_epollInstance < 0) {
			throw std::runtime_error("epoll_create failed");
		}
		struct epoll_event event = {};
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			socket_t socket = it->getSocket();

			event.data.fd = socket;
			event.events = EPOLLIN;
			if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, socket, &event) < 0) {
				throw std::runtime_error("epoll_ctl add failed");
			}
		}
	};
	void StartLoop() {
		if (_epollInstance == 0) {
			InitEpoll();
		}
		if (_isRunning) {
			return;
		}
		_isRunning = true;
		struct epoll_event *events = NULL;
		while (_isRunning) {

			int nfds = epoll_wait(_epollInstance, events, 4096, 200);
			if (nfds < 0) {
				throw std::runtime_error("epoll_wait failed");
			}
			for (int i = 0; i < nfds; i++)
			{
				// if (events[i].events & EPOLLERR)
				Server * serv = FindServerFromSocket(events[i].data.fd);
				if (serv == NULL)
				{
					// NOTE: events[i].data.fd == Client socket

					continue;
				}
				else
					serv->AcceptNewClient(_epollInstance);

				// serv->handleRequest();
			}
		}
	};
	Server	*FindServerFromSocket(socket_t socket) {
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			if (it->getSocket() == socket) {
				return &(*it);
			}
		}
		return NULL;
	};
};
