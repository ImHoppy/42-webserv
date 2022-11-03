/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:37 by cdefonte          #+#    #+#             */
/*   Updated: 2022/10/30 18:31:12 by cdefonte         ###   ########.fr       */
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

class request
{

	private:
		std::string							_raw_rqst; // raw request
	public:
		request(void);
		~request(void);
		request(const request& src);
		request&	operator=(const request& src);
		request(const std::string& str);

		int		set_status_line(void);
		void	split_header(std::string::iterator start, std::string::iterator end);
		int		set_header_fields(void);
		std::string::iterator	find_crlf(std::string::iterator start, std::string::iterator end);
		std::map<std::string, std::string>	_map;
}; // end class request

#endif
