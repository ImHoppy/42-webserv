#include <iostream>
#include <map>
#include "Parsing.hpp"
#include "Logger.hpp"
#include "WebServ.hpp"

#define CONSTRUC


//int main()
//{
//	std::cout << GenerateHtmlDirectory(".") << std::endl;
//	return 0;
//}

#define hw "hi"
void parseConf(GeneralConfig &, std::string const & path );
/*class A {
	public:
	A(){std::cout << "A\n";};
	~A(){std::cout << "~A\n";};
};

class B : public std::string {
	public:
	using std::string::basic_string;
	void test() {
		*this = ("new");
	};
};
 */
int main(int ac, char **av)
{
	std::string		confFile;
	if (ac == 1)
		confFile = "template.conf";
	else
		confFile = av[0];
		
	GeneralConfig generalConfig; // NOTE: Maybe delete this class and put all vector<ServerConfig> in WebServ class or main
	try
	{
		
		parseConf(generalConfig, "template.conf");
	}
	catch(ParsingError& e)
	{
		std::cerr << e.what() << '\n';
		int line = e.whatLine();
		if (line != -1)
			std::cerr << "At line: " << line << '\n';
		exit(1);
	}
	WebServ webserv;
	//TODO:: generalConf rename Servers en configs
	std::vector<ServerConfig>	configs = generalConfig.getServers();
	// check for HOST::PORT pairs (dispatching virtual servers en gros)
	for (std::vector<ServerConfig>::const_iterator confs = configs.begin(); confs != configs.end(); ++confs)
	{
		std::vector<Server*>::iterator	checkForSamePort = webserv.checkIpPort(*confs);
		if (webserv.getServers().empty() == true || checkForSamePort == webserv.getServers().end()) // pas trouve
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
		(*servers)->InitServer();
	Logger::Info("Server started");
	webserv.StartLoop();
	Logger::Info("Server end");

}

