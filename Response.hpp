#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include "ServerConfig.hpp"
# include "Request.hpp"

class Response
{
	public:
		typedef std::map<std::string, std::string>	headers_t;
	private:
		ServerConfig*						_config;
		Request*							_rqst;
		std::map<std::string, std::string>	_headers;
		std::string							_response;

	public:
		/* COplien */
		Response(void);
		~Response(void);
		Response(const Response& src);
		Response&	operator=(const Response& src);
		Response(ServerConfig* config, Request* request);
	
}; // end class Response

std::ostream&	operator<<(std::ostream& o, const Request& me);

#endif
