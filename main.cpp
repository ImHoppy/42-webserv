#include <iostream>
#include <map>
#include "Parsing.hpp"

#define hw "hi"
void	parseConf(std::string const & path );
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
#include <vector>
int main(int ac, char **av)
{

	(void)ac;
	(void)av;
	int i = 1;
	std::cout << hw << i << "\n";

	std::cout << std::endl;

	std::string path(".");
/* 	std::vector<std::string> listFiles = Path::listFiles(path);
	std::cout << "listFiles: " << std::endl;
	for (std::vector<std::string>::iterator it = listFiles.begin(); it != listFiles.end(); it++)
		std::cout << *it << " ";
	std::cout << std::endl << listFiles.size() << std::endl;
	std::cout << std::endl << std::endl; */
	try
	{
		
		parseConf("template.conf");
	}
	catch(ParsingError& e)
	{
		std::cerr << e.what() << '\n';
		int line = e.whatLine();
		if (line != -1)
			std::cerr << "At line: " << line << '\n'; 
	}
	
}

