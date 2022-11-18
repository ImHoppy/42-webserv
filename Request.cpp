/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/28 12:46:48 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/18 12:51:32 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Request.hpp"

/* Default constructor (private) */
Request::Request(void) : _statusCode(200), _rawRqst(), _uri(), _headers(), _body(), _rqstLine() {}

/* Destructor */
Request::~Request(void) {}

/* Copy constructor */
Request::Request(const Request& src) : _statusCode(src._statusCode),
_rawRqst(src._rawRqst),
_uri(src._uri), _headers(src._headers), _body(src._body), _rqstLine(src._rqstLine) {}

/* Assignment operator */
Request&	Request::operator=(const Request& src)
{
	_rawRqst.assign(src._rawRqst);
	_headers.clear();
	_headers = src._headers;
	_body.assign(src._body);
	_statusCode = src._statusCode;
	_uri.scheme = src._uri.scheme;
	_uri.authority = src._uri.authority;
	_uri.path = src._uri.path;
	_uri.query = src._uri.query;
	_uri.total = src._uri.total;
	_rqstLine = src._rqstLine;
	return *this;
}

const std::string &		Request::getRequestLine(void) const
{
	return _rqstLine;
}

/* ATTENTION: str MUST NOT be empty! */
/* Parametric construcotr */
Request::Request(std::string str) : _statusCode(200), _rawRqst(str), _uri(), _headers(), _body(), _rqstLine()
{
	siterator_t		curr;
	if (_statusCode == 200)
		splitRequestLine();
	if (_statusCode == 200)
		curr = setHeaders();
	if (_statusCode == 200)
		parse_Request_target();
	if (_statusCode == 200)
		checkHostHeader();
	if (_statusCode == 200 && curr < _rawRqst.end())
	{
		_body.assign(curr, _rawRqst.end());
//		check_body_headers();
	}
}

//void	Request::check_body_headers(void)
//{
//	std::string::size_type	size = _body.size();
//	headers_t::iterator			te_hdr = _headers.find("Transfer-Encoding");
//	headers_t::iterator			cl_hdr = _headers.find("Content-Length");
//	headers_t::iterator			notfound = _headers.end();
//
//}

/*
USELESS better si la reponse s'en charge non?
Returns true if there must be a message body, false otherwise. (doesnt check if there IS
a message body, juste check if there should be one). Pour l'instant, si TE est present 
alors on renvoi 501 (Not implemented) (en gros accepte que les CL, pour l'instant)
(RFC 7230 page 30).
The presence of a message body in a Request is signaled by a
Content-Length or Transfer-Encoding header field. (RFC 7230 page 28)
A sender MUST NOT send a Content-Length header field in any message
that contains a Transfer-Encoding header field. (RFC 7230 page 30)
If a Transfer-Encoding header field
is present in a Request and the chunked transfer coding is not
the final encoding, the message body length cannot be determined
reliably; the server MUST respond with the 400 (Bad Request)
status code and then close the connection. (RFC 7230 page 32)
*/
bool		Request::message_body_presence(void)
{
	headers_t::iterator		found_cl;
	headers_t::iterator		found_te;
	headers_t::iterator		notfound = _headers.end();
	found_cl = _headers.find("Content-Length");
	found_te = _headers.find("Transfer-Encoding");
	
	if (found_te != notfound && found_cl != notfound)
		return (_statusCode = 400, perror("Request: both Content-Length and Transfer-Encoding present"), false);
	if (found_te != notfound)
		return (_statusCode = 501, perror("Request: Transfer-Encoding not implemented"), false);
	else if (found_cl != notfound)
		return (true);
	return (false);
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
The most common form of Request-target is the origin-form:
	origin-form= absolute-path [ "?" query ]
(RFC 7230 page 42)
else absolute form of URI (http-URI)
(RFC 7230 page 45)
https://fullstack.wiki/http/Request-uri
Remplit la structure t_uri avec les composants de la Request target.
*/
int		Request::parse_Request_target(void)
{
	std::string		target = _headers["Request_target"];
	if (target.empty())
		return (_statusCode = 400, perror("Request: empty target"), -1);
	if (target[0] == '/') // origin form
		parse_origin_form(target);
	else
	{
		parse_absolute_form(target);
		if (_uri.authority != _headers["Host"])
		{
			perror("Request: uri authority mismatch Host header field");
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
int		Request::parse_absolute_form(const std::string& target)
{
	if (target.size() <= 8)
		return (_statusCode = 400, perror("Request: wrong scheme"), -1);
	_uri.scheme.assign(target.begin(), target.begin() + 7);
	if (_uri.scheme != "http://")
		return (_statusCode = 400, perror("Request: wrong scheme"), -1);
	std::string::size_type		auth_end = target.find('/', 7);
	if (auth_end == std::string::npos || auth_end == 7)
		return (_statusCode = 400, perror("Request: wrong Request target"), -1);
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
int		Request::parse_origin_form(const std::string& target)
{
	_uri.scheme = "http://";
	headers_t::iterator		host = _headers.find("Host");
	if (host != _headers.end())
		_uri.authority.assign(host->second);
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

const t_uri&		Request::getUri(void) const
{
	return _uri;
}

/*
A client MUST send a Host header field in all HTTP/1.1 Request
messages. If the target URI includes an authority component, then a
client MUST send a field-value for Host that is identical to that
authority component. 
A server MUST respond with a 400 (Bad Request) 
status code to any
HTTP/1.1 Request message that lacks a Host header field and to any
Request message that contains more than one Host header field or a
Host header field with an invalid field-value. (RFC 7230 page 44)
*/
/*
Doesn't check if there is more than one Host header field, because already checked
in split_header() fct.
*/
int		Request::checkHostHeader(void)
{
	if (_headers.find("Host") == _headers.end()) // si pas trouve
	{
		perror("Request: missing Host header field");
		this->_statusCode = 400;
		return (-1);
	}
	return (0);
}

/* 
In the interest of robustness, servers SHOULD ignore any empty
line(s) received where a Request-Line is expected. In other words, if
the server is reading the protocol stream at the beginning of a
message and receives a CRLF first, it should ignore the CRLF.
Request-line = method SP Request-target SP HTTP-version CRLF
(RFC 7230 page 21)
This function extract the Request line from the raw Request. Set statusCode and return -1
if errors encountered. */
/*
The Method token indicates the method to be performed on the
resource identified by the Request-URI. The method is case-sensitive.
TODO: ERROR 501 if method not implemented by the origin server
TODO: ERROR 405 if method known but not allowed for the Requested resource.
(RFC 2616 page 36)
*/
int		Request::splitRequestLine(void)
{
	siterator_t	start = _rawRqst.begin();
	/* ignore and remove firsts CRLF if any */
	while (start != _rawRqst.end() && start == findCRLF(start, _rawRqst.end()))
		start += 2;
	if (start == _rawRqst.end())
		return (_statusCode = 400, perror("Request: status line empty"), -1);
	_rawRqst.erase(_rawRqst.begin(), start);
	start = _rawRqst.begin();
	if ((findCRLF(start, _rawRqst.end()) - start) > STATUS_LINE_MAX_LENGTH)
		return (_statusCode = 414, perror("Request: status line too long"), -1);
	siterator_t	end = start;
	while (end != _rawRqst.end() && *end != ' ')
		++end;
	_headers["method"] = std::string(start, end);
	if (end == _rawRqst.end() || ++end == _rawRqst.end())
		return (_statusCode = 400, perror("Request: line imcomplete"), -1);
	start = end;
	while (end != _rawRqst.end() && *end != ' ')
		++end;
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("Request: line imcomplete"), -1);
	_headers["Request_target"] = std::string(start, end);
	if (end == _rawRqst.end() || ++end == _rawRqst.end())
		return (_statusCode = 400, perror("Request: line imcomplete"), -1);
	start = end;
	end = findCRLF(end, _rawRqst.end());
	if (end == _rawRqst.end())
		return (_statusCode = 400, perror("Request: line imcomplete (need CRLF)"), -1);
	_headers["http_version"] = std::string(start, end);
	if (end + 2 == _rawRqst.end())
		return (_statusCode = 400, perror("Request: missing message fields"), -1);
	end += 2;
	_rqstLine.assign(_rawRqst.begin(), end - 2);
	_rawRqst.erase(_rawRqst.begin(), end);
	return (0);
}

/*
Find in the range [first, last) the first occurence of CRLF ("\r\n"). Return an iterator
to '\r' if found, end otherwise.
*/
Request::siterator_t	Request::findCRLF(siterator_t start, siterator_t end)
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
A server MUST reject any received Request message that contains
whitespace between a header field-name and colon with a response code
of 400 (Bad Request). (RFC 2616 pqge 25)
*/
int	Request::splitHeaders(siterator_t start, siterator_t end)
{
	if (start == end)
		return (0);
	siterator_t		name_end = find(start, end, ':');
	if (name_end == end || name_end == start || std::isspace(*(name_end-1)) == true)
	{
		perror("Request: split_header error (missing name or missing \':\'");
		_statusCode = 400;
		return (-1);
	}
	siterator_t		value_start = name_end + 1;
	while (value_start != end && *value_start == ' ')
		++value_start;
	std::string		name(start, name_end);
	if (_headers.find("Host") != _headers.end() && name == "Host")
	{
		perror("Request: double Host header fields found");
		_statusCode = 400;
		return (-1);
	}
	if (end - value_start > HEADER_VALUE_MAX_LENGTH)
	{
		perror("Request: value field too long");
		_statusCode = 431;
		return (-1);
	}
	_headers[std::string(start, name_end)] = std::string(value_start, end);
	return (0);
}

/* The Host Request-header field (section 14.23) MUST accompany all
HTTP/1.1 Requests. (RFC2616 page 51) .
All
Internet-based HTTP/1.1 servers MUST respond with a 400 (Bad Request)
status code to any HTTP/1.1 Request message which lacks a Host header
field. (RFC 2616 page 129)*/
Request::siterator_t		Request::setHeaders(void)
{
	siterator_t		eof = _rawRqst.end();
	siterator_t		start = _rawRqst.begin();
	while (start < eof)
	{
		siterator_t		hdr_end = findCRLF(start, eof);
		if (hdr_end == eof) // CRLF pas trouve
		{
			perror("Request: missing CRLF to end message field");
			_statusCode = 400;
			return (eof);
		}
		else if (hdr_end == start) // fin header field
			return (start + 2);
		else if (splitHeaders(start, hdr_end) == -1)
			return eof;
		start = hdr_end + 2;
	}
	return start;
}

const std::string&		Request::getRawRequest(void) const
{
	return this->_rawRqst;
}

const Request::headers_t&			Request::getHeaders(void) const
{
	return this->_headers;
}

std::ostream&	operator<<(std::ostream& o, const Request& me)
{
	Request::headers_t		meMap = me.getHeaders();
	o << "______ REQUEST MAP CONTAINS:" << std::endl;
	for (Request::headers_t::iterator it = meMap.begin(); it != meMap.end(); ++it)
	{
		o << "HF name =\'" << it->first << "\' value =\'" << it->second << "\'"<< std::endl;
	}
	o << "______ REQUEST URI IS: \'" << me.getUri().total << "\'" << std::endl;
	return o;
}

std::string		Request::getMethod(void) const
{
	headers_t::const_iterator	it = _headers.find("method");
	if (it != _headers.end())
		return it->second;
	return (std::string("UNDEFINED"));
}

//TODO: provide a case insensitive comparator to the headers map
std::string			Request::getHost(void) const
{
	headers_t::const_iterator	it = _headers.find("Host");
	if (it != _headers.end())
		return it->second;
	return (std::string("UNDEFINED"));
}
