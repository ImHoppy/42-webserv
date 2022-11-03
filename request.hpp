/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:37 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/03 20:53:00 by cdefonte         ###   ########.fr       */
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

typedef struct s_uri
{
	std::string		scheme;
	std::string		authority;
	std::string		path;
	std::string		query;
	std::string		total;
}				t_uri;

class request
{
	public:
		typedef std::map<std::string, std::string>		map_t;
		typedef std::string::iterator					siterator_t;
		typedef std::string::const_iterator				scstiterator_t;

	private:
		/* Attributs */
		int										_statusCode;
		std::string								_rawRqst;
		t_uri									_uri;
		std::map<std::string, std::string>		_msgFields; // contient aussi la status line
		std::string								_body;
		/* Private default constructor */
		request(void);

	public:
		~request(void);
		/* Parametric constructor */
		request&	operator=(const request& src);
		request(const std::string& str);
		request(const request& src);

		/* Getteurs */
		const std::string&		getRawRequest(void) const;
		const map_t&			getMap(void) const;
		const t_uri&			getUri(void) const;
		/* Member functions */
		int		check_host_header(void); // TODO: a finir qd URI parsing OK
		int		set_request_line(void);
		int		split_header(siterator_t start, siterator_t end);
		int		set_header_fields(void);
		siterator_t	find_crlf(siterator_t start, siterator_t end);
		int		parse_request_target(void);
		int		parse_absolute_form(const std::string& target);
		int		parse_origin_form(const std::string& target);

}; // end class request

std::ostream&	operator<<(std::ostream& o, const request& me);

#endif
