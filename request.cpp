/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:48 by cdefonte          #+#    #+#             */
/*   Updated: 2022/10/30 20:24:20 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "request.hpp"

request::request(void) : _raw_rqst(), _map() {}

request::~request(void) {}

request::request(const request& src) : _raw_rqst(src._raw_rqst), _map(src._map) {}

request&	request::operator=(const request& src)
{
	_raw_rqst.assign(src._raw_rqst);
	_map.clear();
	_map = src._map;
	return *this;
}

/* 
In the interest of robustness, servers SHOULD ignore any empty
line(s) received where a Request-Line is expected. In other words, if
the server is reading the protocol stream at the beginning of a
message and receives a CRLF first, it should ignore the CRLF.
Request status-line = Method SP Request-URI SP HTTP-Version CRLF
(RFC 2616 page 35)
*/
/*
Request-URI = "*" | absoluteURI | abs_path | authority
(RFC 2616 page 36 et RFC 2396 pour precisions sur URI)
TODO: decode URI when % HEX HEX appears
TODO: if absoluteURI none (Host header field ignored), but if abs_path => MUST have URI authority in Host header field
TODO: check if host in (absoluteURI | Host header field) is valid (if not, ERROR 400 "Bad request")
*/
/*
The Method token indicates the method to be performed on the
resource identified by the Request-URI. The method is case-sensitive.
TODO: ERROR 501 if method not implemented by the origin server
TODO: ERROR 405 if method known but not allowed for the requested resource.
(RFC 2616 page 36)
*/
int		request::set_status_line(void)
{
	std::string::iterator	start = _raw_rqst.begin();
	while (start != _raw_rqst.end() && *start == '\n') // ignore and remove firsts CRLF if any
		++start;
	if (start == _raw_rqst.end())
		return (perror("request: status line empty"), -1);
	_raw_rqst.erase(_raw_rqst.begin(), start);
	std::string::iterator	end = start;
	while (end != _raw_rqst.end() && *end != ' ')
		++end;
	_map["method"] = std::string(start, end);
	if (end == _raw_rqst.end() || ++end == _raw_rqst.end())
		return (perror("Request line imcomplete"), -1);
	start = end;
	while (end != _raw_rqst.end() && *end != ' ')
		++end;
	if (end == _raw_rqst.end())
		return (perror("Request line imcomplete"), -1);
	_map["request_uri"] = std::string(start, end);
	if (end == _raw_rqst.end() || ++end == _raw_rqst.end())
		return (perror("Request line imcomplete"), -1);
	start = end;
	while (end != _raw_rqst.end() && *end != '\n')
		++end;
	if (end == _raw_rqst.end())
		return (perror("Request line imcomplete (need CRLF)"), -1);
	_map["http_version"] = std::string(start, end);
	return (0);
}

/* ATTENTION: str MUST NOT be empty! */
request::request(const std::string& str) : _raw_rqst(str), _map()
{
	if (set_status_line() == -1)
		std::cerr << "request: bad status line" << std::endl; // TODO: faire un try catch??
	if (set_header_fields() == -1)
		std::cerr << "request: bad header fields" << std::endl; // TODO: faire un try catch??
//	for (std::map<std::string, std::string>::const_iterator it = _map.begin(); it != _map.end(); ++it)
//		std::cout << it->first << " = " << it->second <<std::endl;

}

/*
field-name ":" [ field-value ] (RFC 2616 page 32)
*/
void	request::split_header(std::string::iterator start, std::string::iterator end)
{
	if (start == end)
		return ;
	std::string::iterator		name_end = find(start, end, ':');
	if (name_end == end)
	{
		perror("request: header field name without value");
		return ;
	}
	std::string::iterator		value_start = name_end + 1;
	while (value_start != end && *value_start == ' ')
		++value_start;
	_map[std::string(start, name_end)] = std::string(value_start, end);
}

/*
Find in the range [first, last) the first occurence of CRLF ("\r\n"). Return an iterator
to '\r' if found, end otherwise.
*/
std::string::iterator	request::find_crlf(std::string::iterator start, std::string::iterator end)
{
	while (start != end)
	{
		if (*start == '\r' && (start + 1) != end && *(start + 1) == '\n')
			return start;
		++start;
	}
	return end;
}

/* The Host request-header field (section 14.23) MUST accompany all
HTTP/1.1 requests. (RFC2616 page 51) */
int		request::set_header_fields(void)
{
	std::string::iterator		eof = _raw_rqst.end();
	std::string::iterator		crlf = find_crlf(_raw_rqst.begin(), eof);
	if (crlf == eof)
		return (perror("request: No header fields"), -1);
	std::string::iterator		start = crlf + 2;
// TODO: if start == eof, then bad request format!!
//	if (start == eof)
//		return (perror("request: No header fields"), -1);
//TODO: ATTENTION: message body avant eof!
	while (start < eof)
	{
		std::string::iterator		hdr_end = find_crlf(start, eof);
		if (hdr_end == eof)
			return (perror("request: missing CRLF to message field"), -1);
//TODO; if (start == hdr_end) => n est dsd message body
		split_header(start, hdr_end);
		start = hdr_end + 2;
	}
	return (0);
}
