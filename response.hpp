/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 11:48:00 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/04 15:50:28 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <map>
# include "request.hpp"

class	response
{
	public:
		response(void);
		~response(void);
		response(const response& src);
		response&	operator=(const response& src);
		response(const std::string& raw_rqst);
		void	set_status_line(void);

	private:
		request								_rqst;
		std::string							_rsp;
		//TODO: ajouter un tableau de configured errors body
		status code | phrase | body

}; // class response

#endif
