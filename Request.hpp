/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:37 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/18 12:49:51 by cdefonte         ###   ########.fr       */
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
# ifndef STATUS_LINE_MAX_LENGTH
#  define STATUS_LINE_MAX_LENGTH 8000 // en octets RFC7230 page 22
# endif
# ifndef HEADER_VALUE_MAX_LENGTH
#  define HEADER_VALUE_MAX_LENGTH 400 // en octets RFC 2616 page 26
# endif

typedef struct s_uri
{
	std::string		scheme;
	std::string		authority;
	std::string		path;
	std::string		query;
	std::string		total;
}				t_uri;

class Request
{
	public:
		typedef std::map<std::string, std::string>		headers_t;
		typedef std::string::iterator					siterator_t;
		typedef std::string::const_iterator				scstiterator_t;

	private:
		/* Attributs */
		int										_statusCode;
		std::string								_rawRqst;
		t_uri									_uri;
		std::map<std::string, std::string>		_headers;
		std::string								_body;
		std::string								_rqstLine; // for debug/info printings
		/* Private default constructor */
		Request(void);

	public:
		~Request(void);
		/* Parametric constructor */
		Request&	operator=(const Request& src);
		Request(std::string str);
		Request(const Request& src);

		/* Getteurs */
		const std::string&			getRawRequest(void) const;
		const headers_t&			getHeaders(void) const;
		const t_uri&				getUri(void) const;
		std::string					getMethod(void) const;
		std::string					getHost(void) const;
		const std::string &			getRequestLine(void) const;
	private:
		int				checkHostHeader(void); // TODO: a finir qd URI parsing OK
		int				splitRequestLine(void);
		int				splitHeaders(siterator_t start, siterator_t end);
		siterator_t		setHeaders(void);
		siterator_t		findCRLF(siterator_t start, siterator_t end);
		int				parse_Request_target(void);
		int				parse_absolute_form(const std::string& target);
		int				parse_origin_form(const std::string& target);
		bool			message_body_presence(void);

}; // end class Request

std::ostream&	operator<<(std::ostream& o, const Request& me);

#endif
