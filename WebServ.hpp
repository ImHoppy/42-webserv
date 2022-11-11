#pragma once

#include "ServerConfig.hpp"
#include "Server.hpp"

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include "Request.hpp"
#include "Client.hpp"

#define MAX_EVENTS 100 // NOTE: 4096

/*
	Listen sockets:
		POLLIN =>	accept connection = Server(lsocket).addClient()
		POLLOUT (pas possible)
	Client sockets:
		POLLIN =>	Client[csocket].addRequest() => Server[csocket].addRequest(csocket) 
					=> Mettre la map de Client dans Server
		POLLOUT =>	Server[csocket].respondClient(csocket);
*/

class WebServ {
	private:
		std::map<socket_t, Server*>				_servers;
		socket_t								_epollInstance;
		bool									_isRunning;
		int										_index;
	public:
		typedef std::map<socket_t, Server*>		map_servers;
		static std::map<socket_t, std::string>	error_status;

	WebServ(): _servers(), _epollInstance(-1), _isRunning(false), _index(0) {};

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
		for (map_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			const socket_t socket = it->first;

			if (_epollInstance > 0)
			{ 
				if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, socket, NULL) < 0) {
					throw std::runtime_error("epoll_ctl del failed");
				}
			}
			if (socket >= 0)
				close(socket);
		}
		if (_epollInstance > 0)
			close(_epollInstance);
	};

	void addServer(Server *serv) {
		if (serv->getSocket() != -1)
		{
			_servers.insert(std::make_pair(serv->getSocket(), serv));
			serv->setIndex(_index);
			_index++;
		}
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
	   for (map_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			   socket_t socket = it->first;

			   event.data.fd = socket;
			   event.events = EPOLLIN;
			   if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, socket, &event) < 0) {
					   throw std::runtime_error("epoll_ctl add failed");
			   }
	   }
	};

	void StartLoop()
	{
		if (_epollInstance < 0)
			InitEpoll();
		if (_isRunning) {
			return;
		}
		_isRunning = true;
		struct epoll_event events[MAX_EVENTS];
		int nfds = 0;
		int			prev_nb = 0;
		while (_isRunning) {
			nfds = epoll_wait(_epollInstance, events, MAX_EVENTS, -1);

			if (nfds < 0) {
				throw std::runtime_error("epoll_wait failed");
			}
			for (int i = 0; i < nfds; i++)
			{
				map_servers::iterator	it = _servers.find(events[i].data.fd);
				if (it == _servers.end())
					continue ;
				Server *serv = it->second;
				if (serv->getSocket() != events[i].data.fd) // Pas un listen socket
				{
					if (events[i].events & EPOLLIN)
					{
						if (serv->recvRequest(events[i].data.fd, _epollInstance) == 0)
							_servers.erase(events[i].data.fd);
					}
					if (events[i].events & EPOLLOUT)
					{
						serv->respond(events[i].data.fd);
					}
				}
				else // is a listen socket
				{
					std::cout << "accept on lsocket " << serv->getSocket() << std::endl;
					socket_t clientFd = serv->AcceptNewClient(_epollInstance);
					_servers.insert(std::make_pair(clientFd, serv));
				}
			}
			prev_nb = nfds;
		}
	};
}; // end class Webserv
