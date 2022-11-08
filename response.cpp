/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 11:51:04 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/08 10:51:16 by Cyrielle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "response.hpp"

/* Default constructor */
response::response(void) : _rqst(), _rsp("HTTP/1.1 ") {}

/* Destructor */
response::~response(void) {}

/* Copy constructor */
response::response(const response& src) : _rqst(src._rqst), _rsp(src._rsp) {}

/* Assignement operator */
response&	response::operator=(const response& src)
{
	_rqst = src._rqst;
	_rsp.assign(src._rsp);
	return *this;
}

void	response::set_rsp_error(int code)
{
//	_rsp += error_pages[code];
}

/* Parametric constructor */
response::response(const std::string& raw_rqst) : _rqst(raw_rqst), _rsp("HTTP/1.1 ")
{
	set_status_line();	
	//TODO: set the _rsp string 
}

/* Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF */
void	response::set_status_line(void)
{
	int		status = _rqst.getSatus();
	_rsp += status + " ";
	if (status != 200)
	{
		set_rsp_error(status);
	}
	else if (status == 200)
		_rsp += "OK";
}
