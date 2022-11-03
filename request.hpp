/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:37 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/03 17:36:13 by cdefonte         ###   ########.fr       */
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
		std::map<std::string, std::string>		_statusLine;
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
		/* Member functions */
		int		set_status_line(void);
		int		split_header(siterator_t start, siterator_t end);
		int		set_header_fields(void);
		siterator_t	find_crlf(siterator_t start, siterator_t end);

}; // end class request

std::ostream&	operator<<(std::ostream& o, const request& me);

#endif
