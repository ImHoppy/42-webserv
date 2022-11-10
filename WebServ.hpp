#pragma once

#include "ServerConfig.hpp"
#include "Server.hpp"

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include "Request.hpp"
#include "Client.hpp"

#define MAX_EVENTS 100 // NOTE: 4096

class WebServ {
	private:
		std::vector<Server>	_servers;
		socket_t			_epollInstance;
		bool				_isRunning;
		std::map<socket_t, Client&>		_clients; // Map de key = client socket, value = Ref sur client
	public:
		typedef std::vector<Server> vec_servers;
		static std::map<socket_t, std::string>	error_status;
	void CreateServer() {
		// Server *serv = new Server();
		// _servers[serv->getSocket()] = serv;
	};

	WebServ(): _servers(), _epollInstance(-1), _isRunning(false), _clients() {
	};

	WebServ(WebServ const & other) {
		*this = other;
	};

	WebServ & operator=(WebServ const & other) {
		if (this != &other) {
			_servers = other._servers;
			_epollInstance = other._epollInstance;
			_isRunning = other._isRunning;
			_clients = other._clients;
		}
		return *this;
	};

	~WebServ() {
	//TODO: ici, on ferme QUE les client_sockets qui sont OK epoll, mais il peut y en avoir 15000 ouverts mais pas ready qu'on ferme pas
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			const socket_t socket = it->getSocket();

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
				Server * serv = FindServerFromSocket(events[i].data.fd);
				if (serv == NULL)
				{
					// NOTE: events[i].data.fd == Client socket
//					if (events[i].events & EPOLLHUP) // voir note plus bas sur Connection close 
//					{
//						std::cout << "EPOLLHUP event (peer closed its end)" << std::endl;
//						if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0) {
//							throw std::runtime_error("epoll_ctl DEL after EOF failed");
//						}
//						close(events[i].data.fd);
//					}
					if (events[i].events & EPOLLIN)
					{
						std::cout << "POLLIN event on client fd " << events[i].data.fd <<std::endl;
						char buf[3000];
						memset(&buf, 0, sizeof(buf));
						ssize_t bytes = recv(events[i].data.fd, buf, 1024, 0);
						if (bytes < 0)
							throw std::runtime_error("recv failed");
						else if (bytes == 0)
						{
							std::cout << "EOF received from client " << events[i].data.fd << std::endl;
							if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0) {
								throw std::runtime_error("epoll_ctl DEL after EOF failed");
							}
							close(events[i].data.fd);
						}
						else if (bytes > 0)
						{
							buf[bytes] = 0;
							std::cout << bytes << "bytes reveived from client " << events[i].data.fd << ": \n";
							std::cout << buf << "______END" << std::endl;	
							//TODO: create la Request avec le buff recu et "l'envoyer" au bon Server
							Request		new_reqst(buf);
							Request::map_t		fields = new_reqst.getMap();
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
//								else if (fields["Connection"] == "Close") // NOTE: en fait sert a rien, car si tu envoi une reponse avec Connection = close, le client declenchera au prochain tour de boucle un POLLIN avec un res du read a 0 bytes, et cest la du coup quon close la co
//								{
//									std::cout << "Close Acknoledgement received from client " << events[i].data.fd << std::endl;
//									if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, events[i].data.fd, NULL) < 0)
//										throw std::runtime_error("epoll_ctl del failed");
//									close(events[i].data.fd);
//								}
							}
						}
					}
				}
				else
				{
					std::cout << "accept on lsocket " << events[i].data.fd << std::endl;
					serv->AcceptNewClient(_epollInstance);
				}
				// serv->handleRequest();
			}
			prev_nb = nfds;
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
