#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include "ServerConfig.hpp"
# include "Request.hpp"
# include "Utils.hpp"
# include "GeneralConfig.hpp"

/*
	_targetPath: path to file or directory. Function request URI path and config root
*/
class Response
{
	public:
		typedef std::map<std::string, std::string>	headers_t;
	private:
		/* Attributs */
		ServerConfig*						_config;
		LocationConfig*						_location;
		Request*							_rqst;
		std::string							_response;
		std::string							_targetPath;
		/* Private member fcts */
		void		setTargetPath(void);

	public:
		/* COplien */
		Response(void);
		~Response(void);
		Response(const Response& src);
		Response&	operator=(const Response& src);
		Response(ServerConfig* config, LocationConfig* loc, Request* request);
		/* Getteurs */
		std::string		getResponse(void) const;
		/* Public Member functions */
		bool	checkHost(void) const;
		bool	checkMethod(void) const;
		void	doGET(void);
		void	doDELETE(void);
		void	doPOST(void);
		bool	tryFile(void);
	
}; // end class Response

std::ostream&	operator<<(std::ostream& o, const Response& me);

#endif