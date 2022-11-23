/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:37 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/22 10:15:23 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
# ifndef STATUS_LINE_MAX_LENGTH
#  define STATUS_LINE_MAX_LENGTH 8000 // en octets RFC7230 page 22
# endif
# ifndef HEADER_VALUE_MAX_LENGTH
#  define HEADER_VALUE_MAX_LENGTH 400 // en octets RFC 2616 page 26
# endif

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
		std::string		_target;
		t_uri			_uri;
		headers_t		_headers;
		std::string		_body;
		/* Private default constructor */
		Request(void);

	public:
		~Request(void);
		Request&	operator=(const Request& src);
		Request(const Request& src);
		/* Parametric constructor */
		Request(const std::string& str);

		/* Getteurs */
		const std::string &			getRawRequest(void) const;
		const std::string &			getRequestLine(void) const;
		const std::string &			getMethod(void) const;
		const std::string &			getTarget(void) const;
		const t_uri &				getUri(void) const;
		const headers_t &			getHeaders(void) const;
		const std::string &			getHost(void) const;
		const std::string &			getBody(void) const;

		void						appendToBody(const std::string & more);

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
