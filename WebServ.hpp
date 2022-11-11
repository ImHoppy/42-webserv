#pragma once

#include "ServerConfig.hpp"
#include "Server.hpp"

#include <vector>
#include <sys/epoll.h>
#include <unistd.h>
#include "Request.hpp"
#include "Client.hpp"
#include <csignal>

#define MAX_EVENTS 100 // NOTE: 4096

//bool	endServer = false;

void	sig_handler(int sig);

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
		std::vector<Server*>				_servers;
		socket_t								_epollInstance;
	public:
		static bool								_isRunning;
		typedef std::vector<Server*>		vec_servers;
		static std::map<socket_t, std::string>	error_status;

	WebServ(): _servers(), _epollInstance(-1) 
	{
	};

	WebServ(WebServ const & other) {
		*this = other;
	};

	WebServ & operator=(WebServ const & other) {
		if (this != &other) {
			_servers = other._servers;
			_epollInstance = other._epollInstance;
		}
		return *this;
	};

	~WebServ() {
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it) {
			const socket_t socket = (*it)->getSocket();

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
			_servers.push_back(serv);
		}
	};

	void	EndLoop(void)
	{
		WebServ::_isRunning = false;
	}

	void	InitEpoll(void)
	{
		if (_servers.size() == 0)
			throw std::runtime_error("No servers to listen to");
		_epollInstance = epoll_create(_servers.size());
		if (_epollInstance < 0)
			throw std::runtime_error("epoll_create failed");
		struct epoll_event event = {};
		for (vec_servers::iterator it = _servers.begin(); it != _servers.end(); ++it)
		{
			Server*	server = *it;
			server->setEpollInstance(_epollInstance);
			t_polldata* data = new t_polldata;
			data->server = server;
			data->client = NULL;
			event.data.ptr = data; 
			event.events = EPOLLIN;
			if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, server->getSocket(), &event) < 0) {
				   throw std::runtime_error("epoll_ctl add failed");
			}
		}
	}

	void StartLoop()
	{
		if (_epollInstance < 0)
			InitEpoll();
		if (WebServ::_isRunning) {
			return;
		}
		WebServ::_isRunning = true;
		struct epoll_event events[MAX_EVENTS];
		int nfds = 0;
		std::signal(SIGINT, sig_handler);
		while (WebServ::_isRunning) {
			nfds = epoll_wait(_epollInstance, events, MAX_EVENTS, -1);
			if (nfds < 0) {
//				throw std::runtime_error("epoll_wait failed");
				perror("WebServ: epoll_wait failed");
				WebServ::_isRunning = false;
				continue ;
			}
			for (int i = 0; i < nfds; i++)
			{
				t_polldata*	data = reinterpret_cast<t_polldata*>(events[i].data.ptr);
				Server	*server = data->server;
				Client	*client = data->client;
				if (client != NULL)
				{
					if (events[i].events & EPOLLERR)
					{
						std::cout << "ERROR on socket " << client->getSocket() <<std::endl;
					}
					if (events[i].events & EPOLLRDHUP)
					{
						std::cout << "POLLRDHUP on socket " << client->getSocket()  <<std::endl;
					}
					if (events[i].events & EPOLLIN)
					{
						int readSize = client->recvRequest();
						if (readSize == 0)
							client->getServer()->removeClient(client);
					}
					if (events[i].events & EPOLLOUT)
					{
						client->getServer()->respond(client);
					}
				}
				else if (server != NULL)
				{
					std::cout << "accept on lsocket " << server->getSocket() << std::endl;
					server->AcceptNewClient();
				}
			}
		}
	};
};// end class Webserv

bool WebServ::_isRunning = false;

void	sig_handler(int sig)
{
	(void)sig;
	std::cout << "SIG HANDLER REACHED" << std::endl;
	WebServ::_isRunning = false;
}
