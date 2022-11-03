/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:48 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/03 17:38:48 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "request.hpp"

/* Default constructor (private) */
request::request(void) : _statusCode(200), _rawRqst(), _msgFields(), _body() {}

/* Destructor */
request::~request(void) {}

/* Copy constructor */
request::request(const request& src) : _statusCode(src._statusCode), _rawRqst(src._rawRqst), _msgFields(src._msgFields), _body(src._body) {}

/* Assignment operator */
request&	request::operator=(const request& src)
{
	_rawRqst.assign(src._rawRqst);
	_msgFields.clear();
	_msgFields = src._msgFields;
	_body.assign(src._body);
	_statusCode = src._statusCode;
	return *this;
}

/* ATTENTION: str MUST NOT be empty! */
/* Parametric construcotr */
request::request(const std::string& str) : _statusCode(200), _rawRqst(str), _msgFields(), _body()
{
	if (_statusCode == 200)
		set_status_line();
	if (_statusCode == 200)
		set_header_fields();
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
	siterator_t	start = _rawRqst.begin();
	/* ignore and remove firsts CRLF if any */
	while (start != _rawRqst.end() && start == find_crlf(start, _rawRqst.end()))
		start += 2;
	if (start == _rawRqst.end())
		return (_statusCode = 400, perror("request: status line empty"), -1);
	_rawRqst.erase(_rawRqst.begin(), start);
	start = _rawRqst.begin();
	if ((find_crlf(start, _rawRqst.end()) - start) > STATUS_LINE_MAX_LENGTH)
		return (_statusCode = 414, perror("request: status line too long"), -1);
	siterator_t	end = start;
	while (end != _rawRqst.end() && *end != ' ')
		++end;
	_msgFields["method"] = std::string(start, end);
	if (end == _rawRqst.end() || ++end == _rawRqst.end())
		return (_statusCode = 400, perror("Request line imcomplete"), -1);
	start = end;
	while (end != _rawRqst.end() && *end != ' ')
		++end;
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("Request line imcomplete"), -1);
	_msgFields["request_uri"] = std::string(start, end);
	if (end == _rawRqst.end() || ++end == _rawRqst.end())
		return (_statusCode = 400, perror("Request line imcomplete"), -1);
	start = end;
	end = find_crlf(end, _rawRqst.end());
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("Request line imcomplete (need CRLF)"), -1);
	_msgFields["http_version"] = std::string(start, end);
	return (0);
}

/*
field-name ":" [ field-value ] (RFC 2616 page 32)
*/
int	request::split_header(siterator_t start, siterator_t end)
{
	if (start == end)
		return (0);
	siterator_t		name_end = find(start, end, ':');
	if (name_end == end || name_end == start)
	{
		perror("request: split_header error");
		_statusCode = 400;
		return (-1);
	}
	siterator_t		value_start = name_end + 1;
	while (value_start != end && *value_start == ' ')
		++value_start;
	_msgFields[std::string(start, name_end)] = std::string(value_start, end);
	return (0);
}

/*
Find in the range [first, last) the first occurence of CRLF ("\r\n"). Return an iterator
to '\r' if found, end otherwise.
*/
request::siterator_t	request::find_crlf(siterator_t start, siterator_t end)
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
HTTP/1.1 requests. (RFC2616 page 51) .
All
Internet-based HTTP/1.1 servers MUST respond with a 400 (Bad Request)
status code to any HTTP/1.1 request message which lacks a Host header
field. (RFC 2616 page 129)*/
int		request::set_header_fields(void)
{
	siterator_t		eof = _rawRqst.end();
	siterator_t		crlf = find_crlf(_rawRqst.begin(), eof);
	if (crlf == eof || crlf + 2 >= eof || std::isspace(*(crlf + 2)))
		return (_statusCode = 400, perror("request: No header fields"), -1);
	siterator_t		start = crlf + 2;
	while (start < eof)
	{
		siterator_t		hdr_end = find_crlf(start, eof);
		if (hdr_end == eof || start == hdr_end)
			return (_statusCode = 400, perror("request: missing CRLF to message field"), -1);
		if (split_header(start, hdr_end) == -1)
			return (-1);
		start = hdr_end + 2;
	}
	return (0);
}



const std::string&		request::getRawRequest(void) const
{
	return this->_rawRqst;
}

const request::map_t&			request::getMap(void) const
{
	return this->_msgFields;
}

std::ostream&	operator<<(std::ostream& o, const request& me)
{
	request::map_t		meMap = me.getMap();
	for (request::map_t::iterator it = meMap.begin(); it != meMap.end(); ++it)
	{
		o << "HF name =\'" << it->first << "\' value =\'" << it->second << "\'"<< std::endl;
	}
	return o;
}
