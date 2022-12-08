#include <iostream>
#include <map>
#include "Parsing.hpp"
#include "Logger.hpp"
#include "WebServ.hpp"
#include <deque>

#define CONSTRUC

/* Return true if there is any Host at 0.0.0.0, false otherwise (all configs hae different IPs). */
bool	isAnyIp(const std::deque<ServerConfig> & confs)
{
	for (std::deque<ServerConfig>::const_iterator it = confs.begin(); it != confs.end(); ++it)
	{
		if (it->getHost() == 0)
			return true;
	}
	return false;
}


int main(int ac, char **av)
{
	std::string		confFile;
	if (ac == 1)
		confFile = "template.conf";
	else
		confFile = av[1];
		
	GeneralConfig generalConfig; // NOTE: Maybe delete this class and put all vector<ServerConfig> in WebServ class or main
	try
	{
		parseConf(generalConfig, confFile);
	}
	catch (std::bad_alloc &e)
	{
		Logger::Error("%s", e.what());
		return (1);
	}
	catch(ParsingError& e)
	{
		std::cerr << e.what() << '\n';
		int line = e.whatLine();
		if (line != -1)
			Logger::Error("%s. At line: %d", e.what(), line);
		return (1);
	}
	WebServ webserv;
	std::vector<ServerConfig>	configs = generalConfig.getConfigs();
	// check for HOST::PORT pairs (dispatching virtual servers en gros)
	std::deque<ServerConfig>	all(configs.begin(), configs.end());
	std::deque<ServerConfig>	samePort;
	while (not all.empty())
	{
		std::deque<ServerConfig>::iterator it = all.begin();
		while (it != all.end())
		{
			if (samePort.empty() || samePort.front().getPort() == it->getPort())
			{
				samePort.push_back(*it);
				std::deque<ServerConfig>::iterator torm = it;
				all.erase(torm);
				it = all.begin();
			}
			else
				++it;
		} // on a tous les samePort
		if (isAnyIp(samePort) == true) // there is at least 1 conf with 0.0.0.0 Host
		{
			Server* serv = new Server;
			for (std::deque<ServerConfig>::iterator it = samePort.begin(); it != samePort.end(); ++it)
				serv->addConfig(*it);
			webserv.addServer(serv);
		}
		else
		{
			for (std::deque<ServerConfig>::iterator it = samePort.begin(); it != samePort.end(); ++it)
			{
				std::vector<Server*>::iterator	found = webserv.checkIpPort(*it);
				if (found != webserv.getServers().end()) // IP::PORT deja added
				{
					(*found)->addConfig(*it);
				}
				else
				{
					Server* serv = new Server;
					serv->addConfig(*it);
					webserv.addServer(serv);
				}
			}
		}
		samePort.clear();
	}

	// initialisation de tous les Servers* (mise en listen state)
	for (std::vector<Server*>::const_iterator servers = webserv.getServers().begin(); servers != webserv.getServers().end(); ++servers)
	{
		if ((*servers)->InitServer() == -1)
			return (-1);
	}
	Logger::Info("Server started");
	try {
		webserv.StartLoop();
	}
	catch (CGI::CGIError& ex)
	{
		Logger::Error("CGI execve failed");
		return (-2);
	}
	catch (std::exception& ex)
	{
		Logger::Error("%s", ex.what());
	}
	Logger::Info("Server end");
	return (0);
}

