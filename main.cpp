#include <iostream>
#include <map>
#include "Parsing.hpp"
#include "GeneralConfig.hpp"


#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

std::vector<std::string>	listFiles(std::string const & path)
{
	DIR							*dr;
	struct dirent				*en;
	std::vector<std::string>	vec_files;

	dr = opendir(path.c_str());
	if (dr)
	{
		while ((en = readdir(dr)) != NULL)
		{
			vec_files.push_back(en->d_name);
		}
		closedir(dr);
	}
	return vec_files;
}

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
#include "Server.hpp"
#include "WebServ.hpp"
int main(int ac, char **av)
{

	(void)ac;
	(void)av;
	int i = 1;
	std::cout << hw << i << "\n";

	std::cout << std::endl;

	std::vector<std::string> list = listFiles(".");
	std::cout << "listFiles: " << std::endl;
	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++)
	{
		struct stat st;
		stat(it->c_str(), &st);
		std::cout << *it << " : " << st.st_size <<  "\n";
	}
	std::cout << std::endl << list.size() << std::endl;
	std::cout << std::endl << std::endl;

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
	}
	WebServ webserv;
	Server* serv = new Server;
	serv->addConfig(generalConfig.getServers()[0]);
	serv->InitServer();

	webserv.addServer(serv);
	webserv.StartLoop();
	delete serv;
}

