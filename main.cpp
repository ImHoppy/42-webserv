#include <iostream>
#include <map>
#include "Parsing.hpp"
#include "GeneralConfig.hpp"
#include "Logger.hpp"

#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>

#define CONSTRUC

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

bool	startsWith(std::string const & str, std::string const & start)
{
	if (str.length() < start.length())
		return false;
	return (str.compare(0, start.length(), start) == 0);
}

std::string	GenerateHtmlDirectory(std::string const & path)
{
	std::vector<std::string>	vec_files = listFiles(path);
	std::string					htmlPage;
	std::vector<std::string>::iterator it;

	htmlPage = "<html><head>";
	htmlPage += "<title> /" + path + "</title>";
	htmlPage += "<style type=\"text/css\"> a {display: block; padding: .5em .75em; text-decoration: none; color: #363636; border-bottom: 1px solid #ededed;} a:hover { color: black; background-color: #a6a6a6;}</style>";
	htmlPage += "</head><body bgcolor=\"white\">";
	htmlPage += "<h1>Index of " + path + "</h1>";
	htmlPage += "<a href=\"../\">Parent Directory</a>\n";

	for (it = vec_files.begin(); it != vec_files.end(); it++)
	{
		if (startsWith(*it, "."))
			continue;
		htmlPage += "<a href=\""+ *it +"\">" + *it + "</a>\n";
	}
	htmlPage += "</body></html>";
	return htmlPage;
}

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
//	std::cout << "listFiles: " << std::endl;
//	for (std::vector<std::string>::iterator it = list.begin(); it != list.end(); it++)
//	{
//		struct stat st;
//		stat(it->c_str(), &st);
//		std::cout << *it << " : " << st.st_size <<  "\n";
//	}
//	std::cout << std::endl << list.size() << std::endl;
//	std::cout << std::endl << std::endl;

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
	Server* serv = new Server;
	serv->addConfig(generalConfig.getServers()[0]);
	serv->InitServer();

	Logger::Info("Server started");
	webserv.addServer(serv);
	webserv.StartLoop();
	Logger::Info("Server end");

}

