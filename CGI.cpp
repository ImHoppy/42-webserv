#include "CGI.hpp"

CGI::CGI(void) {}

CGI::~CGI(void) {}

CGI::CGI(const CGI & src) : _env(src._env) {}

CGI &	CGI::operator=(const CGI & src)
{
	_env = src._env;
	return *this;
}

/* Set le vector d'environnement variables en fonction de la requete (RFC 3875) */
void	CGI::setEnv(const Request & rqst)
{
	Request::headers_t		headers = rqst.getHeaders();
	Request::headers_t::const_iterator	not_found = headers.end();

	Request::headers_t::const_iterator	found = headers.find("Authorization");
	if (found != not_found)
	{
		_env.push_back("AUTH_TYPE" + found->second);
	}
	found = headers.find("Content-Type");
	if (found != not_found)
	{
		_env.push_back("CONTENT_TYPE" + found->second);
	}
	found = headers.find("Content-Length");
	if (found != not_found)
	{
		_env.push_back("CONTENT_LENGTH" + found->second);
	}
	_env.push_back("GATEWAY_INTERFACE=CGI/0.1");
	_env.push_back("PATH_INFO=/"); // ou si myscript.php/this/is/pathinfo?query
	_env.push_back("QUERY_STRING=" + rqst.getUri().query);
//	_env.push_back("REMOTE_ADDR=" + inet_ntoa(client->addr())); sauf qu'il faut le client du coup nieh
//	_env.push_back("REMOTE_HOST=" + getclient->addr()); sauf qu'il faut le client du coup nieh
	_env.push_back("REQUEST_METHOD=" + rqst.getMethod());
//TODO: mettre le target path dans Request, pour que ce soit accessible et au CGI et a Response
//	_env.push_back("SCRIPT_NAME=" + rqst.getMethod());
}
