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
	public:
		typedef std::map<socket_t, Server*>		map_servers;
		static std::map<socket_t, std::string>	error_status;

	WebServ(): _servers(), _epollInstance(-1), _isRunning(false) {};

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
	//TODO: ici, on ferme QUE les client_sockets qui sont OK epoll, mais il peut y en avoir 15000 ouverts mais pas ready qu'on ferme pas
		for (map_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			const socket_t socket = it->second->getSocket();

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

	void addServer(Server &serv) {
		if (serv.getSocket() != -1)
			_servers.insert(std::make_pair(serv.getSocket(), &serv));
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
			if (nfds != prev_nb)
			{
				std::cout << "nfds=" << nfds << std::endl;
				for (int j = 0; j < nfds; ++j)
				{
					std::cout << "events[" << j << "] = " << events[j].data.fd << std::endl;
				}
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
						serv->recvRequest(events[i].data.fd);
					}
					if (events[i].events & EPOLLOUT)
					{
						if (fields["method"] == "GET")
						{
							std::cout << "POLLOUT event on client fd " << events[i].data.fd <<std::endl;
							ssize_t		bytes;
							std::string buf("HTTP/1.1 200 OK\r\nContent-Length: 13\r\nConnection: Keep-Alive\r\n\r\nHello, world!");
							bytes = send(events[i].data.fd, buf.c_str(), buf.size(), 0);
							if (bytes == -1)
							{
								throw std::runtime_error("send failed");
							}
							std::cout << bytes << " send to client " << events[i].data.fd << std::endl;
						}
					}
				}
				else // is a listen socket
				{
					std::cout << "accept on lsocket " << serv->getSocket() << std::endl;
					serv->AcceptNewClient(_epollInstance);
				}
			}
			prev_nb = nfds;
		}
	};
}; // end class Webserv
