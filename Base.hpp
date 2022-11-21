#ifndef BASE_HPP
# define BASE_HPP

#include <string>

class Server;
class Client;
typedef struct s_polldata
{
	Server*	server;
	Client*	client;
} t_polldata;

class Base {
	protected:
	std::string	_type;
	public:
	Base(std::string const & type) : _type(type) {};
	virtual ~Base() {};

	virtual std::string const & getType() const = 0;
};

#endif

