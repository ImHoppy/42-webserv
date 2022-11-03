/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:48 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/03 21:19:15 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "request.hpp"

/* Default constructor (private) */
request::request(void) : _statusCode(200), _rawRqst(), _uri(), _msgFields(), _body() {}

/* Destructor */
request::~request(void) {}

/* Copy constructor */
request::request(const request& src) : _statusCode(src._statusCode), _rawRqst(src._rawRqst), _uri(src._uri), _msgFields(src._msgFields), _body(src._body) {}

/* Assignment operator */
request&	request::operator=(const request& src)
{
	_rawRqst.assign(src._rawRqst);
	_msgFields.clear();
	_msgFields = src._msgFields;
	_body.assign(src._body);
	_statusCode = src._statusCode;
	_uri.scheme = src._uri.scheme;
	_uri.authority = src._uri.authority;
	_uri.path = src._uri.path;
	_uri.query = src._uri.query;
	_uri.total = src._uri.total;
	return *this;
}

/* ATTENTION: str MUST NOT be empty! */
/* Parametric construcotr */
request::request(const std::string& str) : _statusCode(200), _rawRqst(str), _msgFields(), _body()
{
	if (_statusCode == 200)
		set_request_line();
	if (_statusCode == 200)
		set_header_fields();
	if (_statusCode == 200)
		parse_request_target();
//	if (_statusCode == 200)
//		check_host_header();
}

/*
Http URI: (RFC 7230 page 17)
http-URI = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
	The authority component is preceded by a double slash ("//") and is
terminated by the next slash ("/"), question mark ("?"), or number
sign ("#") character, or by the end of the URI.
	The path is terminated
by the first question mark ("?") or number sign ("#") character, or
by the end of the URI.
 If a URI contains an authority component, then the path component
   must either be empty or begin with a slash ("/") character.
  path          = path-abempty    ; begins with "/" or is empty
*/

/*
The most common form of request-target is the origin-form:
	origin-form= absolute-path [ "?" query ]
(RFC 7230 page 42)
else absolute form of URI (http-URI)
(RFC 7230 page 45)
https://fullstack.wiki/http/request-uri
*/
int		request::parse_request_target(void)
{
	std::string		target = _msgFields["request_target"];
	if (target.empty())
		return (_statusCode = 400, perror("request: empty target"), -1);
	if (target[0] == '/') // origin form
		parse_origin_form(target);
	else
	{
		parse_absolute_form(target);
		if (_uri.authority != _msgFields["Host"])
		{
			perror("request: uri authority mismatch Host header field");
			_statusCode = 400;
			return (-1);
		}
	}
	if (_uri.query.empty())
		_uri.total = _uri.scheme + _uri.authority + _uri.path;
	else
		_uri.total = _uri.scheme + _uri.authority + _uri.path + "?" + _uri.query;
	return (0);
}

/*
	absolute-form = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
	path-abempty begins with "/" or is empty
*/
int		request::parse_absolute_form(const std::string& target)
{
	if (target.size() <= 8)
		return (_statusCode = 400, perror("request: wrong scheme"), -1);
	_uri.scheme.assign(target.begin(), target.begin() + 7);
	if (_uri.scheme != "http://")
		return (_statusCode = 400, perror("request: wrong scheme"), -1);
	std::string::size_type		auth_end = target.find('/', 7);
	if (auth_end == std::string::npos || auth_end == 7)
		return (_statusCode = 400, perror("request: wrong request target"), -1);
	_uri.authority.assign(target.begin() + 7, target.begin() + auth_end);
	std::string::size_type	path_end = target.find('?', auth_end);
	if (path_end == std::string::npos)
	{
		_uri.path.assign(target.begin() + auth_end, target.end());
	}
	else
	{
		_uri.path.assign(target.begin() + auth_end, target.begin() + path_end);
		_uri.query.assign(target.begin() + path_end, target.end());
	}
	return (0);
}

/*
	origin-form= absolute-path [ "?" query ]
*/
int		request::parse_origin_form(const std::string& target)
{
	_uri.scheme = "http://";
	_uri.authority.assign(_msgFields["Host"]);
	std::string::size_type	path_end = target.find('?');
	if (path_end == std::string::npos)
	{
		_uri.path.assign(target.begin(), target.end());
	}
	else
	{
		_uri.path.assign(target.begin(), target.begin() + path_end);
		_uri.query.assign(target.begin(), target.end());
	}
	return (0);
}

const t_uri&		request::getUri(void) const
{
	return _uri;
}

/*
A client MUST send a Host header field in all HTTP/1.1 request
messages. If the target URI includes an authority component, then a
client MUST send a field-value for Host that is identical to that
authority component. A server MUST respond with a 400 (Bad Request) 
status code to any
HTTP/1.1 request message that lacks a Host header field and to any
request message that contains more than one Host header field or a
Host header field with an invalid field-value. (RFC 7230 page 44)
*/
/*
Doesn't check if there is more than one Host header field, because already checked
in split_header() fct.
*/
int		request::check_host_header(void)
{
	if (_msgFields.find("Host") == _msgFields.end())
	{
		perror("request: missing Host header field");
		this->_statusCode = 400;
		return (-1);
	}
	//TODO: if Host valur != URI authority error
	//TODO: if URI without authority => Host should be empty val
	return (0);
}

/* 
In the interest of robustness, servers SHOULD ignore any empty
line(s) received where a Request-Line is expected. In other words, if
the server is reading the protocol stream at the beginning of a
message and receives a CRLF first, it should ignore the CRLF.
request-line = method SP request-target SP HTTP-version CRLF
(RFC 7230 page 21)
This function extract the request line from the raw request. Set statusCode and return -1
if errors encountered. */
/*
The Method token indicates the method to be performed on the
resource identified by the Request-URI. The method is case-sensitive.
TODO: ERROR 501 if method not implemented by the origin server
TODO: ERROR 405 if method known but not allowed for the requested resource.
(RFC 2616 page 36)
*/
int		request::set_request_line(void)
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
		return (_statusCode = 400, perror("request: line imcomplete"), -1);
	start = end;
	while (end != _rawRqst.end() && *end != ' ')
		++end;
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("request: line imcomplete"), -1);
	_msgFields["request_target"] = std::string(start, end);
	if (end == _rawRqst.end() || ++end == _rawRqst.end())
		return (_statusCode = 400, perror("request: line imcomplete"), -1);
	start = end;
	end = find_crlf(end, _rawRqst.end());
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("request: line imcomplete (need CRLF)"), -1);
	_msgFields["http_version"] = std::string(start, end);
	if (end + 2 == _rawRqst.end())
		return (_statusCode = 400, perror("request: missing message fields"), -1);
	end += 2;
	_rawRqst.erase(_rawRqst.begin(), end);
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

/*
field-name ":" [ field-value ] (RFC 2616 page 32)
Split start to end with ':' as delimiter, add the resulting key and value in map, and
check if there is not more than one Host header field. Set statusCode and return -1 if
any error encountered.
*/
int	request::split_header(siterator_t start, siterator_t end)
{
	if (start == end)
		return (0);
	siterator_t		name_end = find(start, end, ':');
	if (name_end == end || name_end == start)
	{
		perror("request: split_header error (missing name or missing \':\'");
		_statusCode = 400;
		return (-1);
	}
	siterator_t		value_start = name_end + 1;
	while (value_start != end && *value_start == ' ')
		++value_start;
	std::string		name(start, name_end);
	if (_msgFields.find("Host") != _msgFields.end() && name == "Host")
	{
		perror("request: double Host header fields found");
		_statusCode = 400;
		return (-1);
	}
	_msgFields[std::string(start, name_end)] = std::string(value_start, end);
	return (0);
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
	siterator_t		start = _rawRqst.begin();
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
	o << "______ REQUEST MAP CONTAINS:" << std::endl;
	for (request::map_t::iterator it = meMap.begin(); it != meMap.end(); ++it)
	{
		o << "HF name =\'" << it->first << "\' value =\'" << it->second << "\'"<< std::endl;
	}
	o << "______ REQUEST URI IS: \'" << me.getUri().total << "\'" << std::endl;
	return o;
}
