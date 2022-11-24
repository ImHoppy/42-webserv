#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <cstdio> // std::remove (file/dir)
# include <sys/types.h> // waitpid()
# include <sys/wait.h> // waitpid()
# include <strings.h> // bzero

# include "Utils.hpp"
# include "Logger.hpp"
# include "CGI.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Request.hpp"

#define CLRF "\r\n"




class Client;
class Response
{
	public:
		enum StatusCode { // NOTE: Unused struct
			NONE = 0,
			READY_READ,
			READY_SEND,
			EOF_FILE,
			FAILED
		};
		struct ReadData {
			char			*buffer;
			StatusCode		status; // NOTE: Unused
			unsigned int	read_bytes;
			std::ifstream	file;
		};
		typedef std::map<std::string, std::string>		headers_t;
	private:
		/* Attributs */
		Request*					_rqst;
		// TODO: Move cgi to client
		CGI							_cgi;
		std::pair<int, std::string>	_code;
		headers_t					_headers;
		std::string					_body;
		std::string					_response;
		ReadData					_readData;

		/* Private member fcts */
		void		setAllowHeader(void);
		//TODO: ameliorer avec des private static bitset<3> file dit cgi ?
		bool		targetIsDir(void) const;
		bool		targetIsFile(void) const;
		bool		targetIsCgi(void) const;
		void		phpCgiGet(void);
		void		upload(void);
		void		setCgiEnv(void);
		
		bool	openPageError(std::string path);
		void	generateBodyError();
	public:

		/* Coplien */
		Response(void);
		~Response(void);
		Response(const Response& src);
		Response&	operator=(const Response& src);
		Response(Request* rqst);
		
		/* Getteurs */
		std::string			getResponse(void) const;
		ReadData const &	getReadData(void) const;

		/* Public Member functions */
		bool	checkHost(void) const;
		bool	checkMethod(void) const;
		void	doGET(void);
		int		doDELETE(const std::string &path);
		void	doPOST(void);
		bool	tryFile(void);
		int		readFromCgi(void);
		void	handleError(int error);
		void	generateResponse(void);
		void	doMethod(void);

}; // end class Response

std::ostream&	operator<<(std::ostream& o, const Response& me);

#endif
