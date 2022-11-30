#ifndef  REQUEST_HPP
# define  REQUEST_HPP


# include <algorithm> // find 
# include <iostream>
# include <string>
# include <map>
# include <stdio.h> // perror
# include <errno.h> // perror
# include <cctype> // isspace
# include "Utils.hpp" // pour findCRLF
# include "Logger.hpp"

# ifndef STATUS_LINE_MAX_LENGTH
#  define STATUS_LINE_MAX_LENGTH 8000 // en octets RFC7230 page 22
# endif
# ifndef HEADER_VALUE_MAX_LENGTH
#  define HEADER_VALUE_MAX_LENGTH 400 // en octets RFC 2616 page 26
# endif

/*
	_targetPath: path to file or directory. Function request URI path and config root
*/

#define CLRF "\r\n"

typedef struct s_uri
{
	std::string		scheme; // http://
	std::string		authority; // domain name ou IP [:port]
	std::string		path; // file ou directory 
	std::string		query; // query string (voif html form / CGI
}				t_uri;


class Request
{
	public:
		typedef std::map<std::string, std::string>		headers_t;
		typedef std::string::iterator					siterator_t;
		typedef std::string::const_iterator				scstiterator_t;

	private:
		/* Attributs */
		std::string		_rawRqst;
		std::string		_rqstLine; // for debug/info printings
		std::string		_method;
		std::string		_target; // target brut URL
		std::string		_targetPath; // target rooted fct conf et loc
		t_uri			_uri;
		headers_t		_headers;
		std::string		_body;
		ServerConfig*	_conf;
		LocationConfig*	_loc;
		std::string		_uploadFileName;

		/* Private default constructor */
		Request(void);

	public:
		~Request(void);
		Request&	operator=(const Request& src);
		Request(const Request& src);
		/* Parametric constructor */
		Request(char * buf, size_t & start_buf, ssize_t & bytes);
		/* Public Setteurs */
		void	setLocation(LocationConfig* loc);
		void	setConfig(ServerConfig* conf);

		void						setUploadFile(const std::string & path);
		const std::string &			getUploadFile(void) const;

		/* Getteurs */
		ssize_t						getBodySize(void) const;
		const std::string &			getTargetPath(void) const;
		const std::string &			getRawRequest(void) const;
		const std::string &			getRequestLine(void) const;
		const std::string &			getMethod(void) const;
		const std::string &			getTarget(void) const;
		const t_uri &				getUri(void) const;
		const headers_t &			getHeaders(void) const;
		const std::string &			getHost(void) const;
		const std::string &			getBody(void) const;
		ServerConfig*				getConfig(void) const;
		LocationConfig*				getLocation(void) const;
		std::string					getContentLength(void) const;
		std::string					getValForHdr(const std::string & hdrToFind) const;

		void			appendToBody(const std::string & more);
		bool			targetIsCgi(void) const;
		bool		targetIsFile(void) const;
		bool		targetIsDir(void) const;
		void			setTargetPath(void);

	private:
		/* Setteurs */
		void			setRqstLine(void);
		void			setMethod(void);
		void			setTarget(void);
		int				setURI(void);
		siterator_t		setHeaders(void);
		void			setBody(siterator_t start);
		/* Private member fcts (utils) */
		int				splitHeaders(siterator_t start, siterator_t end);
		int				parse_origin_form(void);
		int				parse_absolute_form(void);

}; // end class Request

std::ostream&	operator<<(std::ostream& o, const Request& me);

#endif
