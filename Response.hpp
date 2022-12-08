#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <iostream>
# include <string>
# include <map>
# include <cstdio> // std::remove (file/dir)
# include <strings.h> // bzero
# include <unistd.h> // lseek

# include "Utils.hpp"
# include "Logger.hpp"
# include "CGI.hpp"
# include "ServerConfig.hpp"
# include "LocationConfig.hpp"
# include "Request.hpp"

#define CLRF "\r\n"

# include "Client.hpp"


class Client;
class Response
{
	public:
		enum StatusCode {
			NONE = 0,
			IMCOMPLETE_READ
		};
		struct ReadData {
			char			*buffer;
			StatusCode		status;
			unsigned int	read_bytes;
			std::ifstream	file;
		};
		typedef std::map<std::string, std::string>		headers_t;
	private:
		/* Attributs */
		Request*					_rqst;
		Client						*_client;
		CGI							_cgi;
		std::pair<int, std::string>	_code;
		headers_t					_headers;
		std::string					_body;
		std::string					_response;
		ReadData					_readData;

		/* Private member fcts */
		void		setAllowHeader(void);
		bool		targetIsDir(void) const;
		bool		targetIsFile(void) const;
		void		upload(void);
		
		bool	openPageError(std::string path);
		void	generateBodyError();

		/* CGI */
		void		setCgiEnv(void);
		void		doCGI(void);
	public:

		/* Coplien */
		Response(void);
		~Response(void);
		Response(const Response& src);
		Response&	operator=(const Response& src);
		Response(Request* rqst, Client* client);
		
		/* Getteurs */
		std::string			getResponse(void) const;
		ReadData const &	getReadData(void) const;
		const std::string &	getBody(void) const;
		const std::pair<int, std::string> &	getCode(void) const;

		/* Public Member functions */
		bool	checkHost(void) const;
		bool	checkMethod(void) const;
		void	doDirectoryListening(void);
		void	doGET(void);
		int		doDELETE(const std::string &path);
		void	doPOST(void);
		bool	tryFile(void);
		int		readFromCgi(void);
		void	handleError(int error);
		void	generateResponse(void);
		void	doMethod(void);
		
		void	UploadMultipart(void);
		bool	checkBodySize(void);

}; // end class Response

std::ostream&	operator<<(std::ostream& o, const Response& me);

#endif
