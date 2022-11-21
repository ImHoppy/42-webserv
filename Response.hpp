#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <cstdio> // std::remove (file/dir)
# include <sys/types.h> // waitpid()
# include <sys/wait.h> // waitpid()

# include "Utils.hpp"
# include "Logger.hpp"
# include "Client.hpp"
# include "CGI.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Request.hpp"

/*
	_targetPath: path to file or directory. Function request URI path and config root
*/

class Client;
class Response
{
	public:
		typedef std::map<std::string, std::string>	headers_t;
	private:
		/* Attributs */
		ServerConfig*						_config;
		LocationConfig*						_location;
		Request*							_rqst;
		Client*								_client;
		std::string							_response;
		std::string							_targetPath;
		CGI									_cgi;
		/* Private member fcts */
		void		setTargetPath(void);
		void		setAllowHeader(void);
		//TODO: ameliorer avec des private static bitset<3> file dit cgi ?
		bool		targetIsDir(void) const;
		bool		targetIsFile(void) const;
		bool		targetIsCgi(void) const;
		void		getFile(void);
		void		phpCgiGet(void);
		void		upload(void);
		void		setCgiEnv(void);

	public:
		/* COplien */
		Response(void);
		~Response(void);
		Response(const Response& src);
		Response&	operator=(const Response& src);
		Response(ServerConfig* config, LocationConfig* loc, Request* request, Client* client);
		/* Getteurs */
		std::string		getResponse(void) const;
		/* Public Member functions */
		bool	checkHost(void) const;
		bool	checkMethod(void) const;
		void	doGET(void);
		int		doDELETE(const std::string &path);
		void	doPOST(void);
		bool	tryFile(void);
	
}; // end class Response

std::ostream&	operator<<(std::ostream& o, const Response& me);

#endif
