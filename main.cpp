#include <iostream>
#include <map>
#include "Parsing.hpp"
#include "Logger.hpp"
#include "WebServ.hpp"

#define CONSTRUC


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
		int line = e.whatLine();
		if (line != -1)
			Logger::Error("%s. At line: %d", e.what(), line);
		else
			Logger::Error("%s.", e.what());
		return (1);
	}
	WebServ webserv;
	//TODO:: generalConf rename Servers en configs
	std::vector<ServerConfig>	configs = generalConfig.getConfigs();
	// check for HOST::PORT pairs (dispatching virtual servers en gros)
	for (std::vector<ServerConfig>::const_iterator confs = configs.begin(); confs != configs.end(); ++confs)
	{
		std::vector<Server*>::iterator	checkForSamePort = webserv.checkIpPort(*confs);
		if (webserv.getServers().empty() || checkForSamePort == webserv.getServers().end()) // pas trouve
		{
			Server* serv = new Server;
			serv->addConfig(*confs);
			webserv.addServer(serv);
		}
		else
		{
			(*checkForSamePort)->addConfig(*confs);
		}
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

