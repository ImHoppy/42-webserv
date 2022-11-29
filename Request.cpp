# include "Request.hpp"

void			Request::appendToBody(const std::string & more)
{
	_body += more;
}

/* Search for a specific header and return either NULL either the string containing the value
associated to this header. */
std::string 		Request::getValForHdr(const std::string & hdrToFind) const
{
	headers_t::const_iterator	it = _headers.find(hdrToFind);
	if (it == _headers.end())
		return "";
	return (it->second);
}

std::string 		Request::getContentLength(void) const
{
	headers_t::const_iterator	it = _headers.find("Content-Length");
	if (it == _headers.end())
		return ("0");
	return it->second;
}

/* Default constructor (private) */
Request::Request(void) {}

/* Destructor */
Request::~Request(void) {
	if (_uploadFileName.empty())
		return;
	if (remove(_uploadFileName.c_str()) != 0)
		Logger::Error("Error deleting file %s", _uploadFileName.c_str());
}

/* Copy constructor */
Request::Request(const Request& src) : 
_rawRqst(src._rawRqst), _rqstLine(src._rqstLine), _method(src._method),
_target(src._target), _targetPath(src._targetPath),
_uri(src._uri), _headers(src._headers), _body(src._body), _uploadFileName(src._uploadFileName) {}


/* Assignment operator */
Request&	Request::operator=(const Request& src)
{
	_rawRqst.assign(src._rawRqst);
	_rqstLine.assign(src._rqstLine);
	_method.assign(src._method);
	_target.assign(src._target);
	_targetPath = src._targetPath;
	_uri.scheme = src._uri.scheme;
	_uri.authority = src._uri.authority;
	_uri.path = src._uri.path;
	_uri.query = src._uri.query;
	_headers = src._headers;
	_body.assign(src._body);
	_uploadFileName = src._uploadFileName;
	return *this;
}

#include <cstring>
/* ATTENTION: str MUST NOT be empty! */
/* Parametric construcotr */
Request::Request(char * buf, size_t & start_buf, ssize_t & bytes)
{
	ssize_t i;
	for (i = 0; i < bytes - 4 ; ++i)
	{
		if (strncmp(buf + i,"\r\n\r\n", 4) == 0)
			break;
	}
	i += 4;
	_rawRqst.assign(buf, buf + i);
	start_buf = i;
	setRqstLine();
	setMethod();
	setTarget();
	siterator_t	bodyStart;
	bodyStart = setHeaders();
	// if (bodyStart != _rawRqst.end())
	// 	_body.assign(buf+i, 0, (bytes - i));
	setURI();
	if (_method == "POST")
		bytes = bytes - i;
}

const std::string &		Request::getRequestLine(void) const
{
	return _rqstLine;
}

const std::string &		Request::getTargetPath(void) const
{
	return _targetPath;
}

bool		Request::targetIsDir(void) const
{
	return (ends_with(_targetPath, '/'));
}

bool		Request::targetIsFile(void) const
{
	return !(targetIsDir() || targetIsCgi());
}

bool		Request::targetIsCgi(void) const
{
	return (ends_with(_targetPath, ".php"));
}

/* Set le private attribut _targetPath en fonction du path de l'URI de la request et en
fonction du root de la config. */
void	Request::setTargetPath(void)
{
	std::string		url(_uri.path);
	_targetPath =  url.replace(0, _loc->getPath().size(), _loc->getRootPath());
	// TODO: Only use url or _targetPath but not both
	if (ends_with(url, '.'))
		_targetPath += '/';
	if (ends_with(_targetPath, '/'))
	{
		if (_loc->isDirList() == true || _method == "DELETE")
			return ;
		_targetPath += _loc->getIndexFile();
	}
}

/* Enregistre le body: prend un iterator qui pointe sur le premier caractere, i.e le
premier caractere apres une ligne "vide" (ne contenant que "\r\n");
*/
void	Request::setBody(siterator_t start)
{
	_body.assign(start, _rawRqst.end());
}

void	Request::setRqstLine(void)
{
	_rqstLine.assign(_rawRqst.begin(), findCRLF(_rawRqst.begin(), _rawRqst.end()));
}

void	Request::setMethod(void)
{
	size_t	pos = _rawRqst.find(' ');
	_method.assign(_rawRqst, 0, pos);
}

/*
	Set la request target : en fait tout l'URI (entre METHOD et VERSION)
*/
void	Request::setTarget(void)
{
	siterator_t		crlf = findCRLF(_rawRqst.begin(), _rawRqst.end());
	siterator_t		tBegin = crlf;
	siterator_t		tEnd = crlf;
	for (siterator_t it = _rawRqst.begin(); it != crlf; ++it)
	{
		if (*it == ' ')
		{
			if (tBegin == crlf)
				tBegin = ++it;
			else
			{
				tEnd = it;
				break ;
			}
		}
	}
	_target.assign(tBegin, tEnd);
}

/*
	http-URI = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
	Remplit la structure t_uri avec les composants de la Request target.
*/
int		Request::setURI(void)
{
	if (_target[0] == '/') // origin form
		parse_origin_form();
	else
	{
		parse_absolute_form();
		if (_uri.authority != _headers["Host"])
		{
			perror("Request: uri authority mismatch Host header field");
			return (-1);
		}
	}
	return (0);
}

/*
	absolute-form = "http:" "//" authority path-abempty [ "?" query ] [ "#" fragment ]
	path-abempty begins with "/" or is empty
*/
int		Request::parse_absolute_form(void)
{
	_uri.scheme.assign(_target.begin(), _target.begin() + 7);
	if (_uri.scheme != "http://")
		return (perror("Request: wrong scheme"), -1);
	std::string::size_type		auth_end = _target.find('/', 7);
	if (auth_end == std::string::npos || auth_end == 7)
		return (perror("Request: wrong Request _target"), -1);
	_uri.authority.assign(_target.begin() + 7, _target.begin() + auth_end);
	std::string::size_type	path_end = _target.find('?', auth_end);
	if (path_end == std::string::npos)
	{
		_uri.path.assign(_target.begin() + auth_end, _target.end());
	}
	else
	{
		_uri.path.assign(_target.begin() + auth_end, _target.begin() + path_end);
		_uri.query.assign(_target.begin() + path_end + 1, _target.end());
	}
	return (0);
}

/*
	origin-form= absolute-path [ "?" query ]
*/
int		Request::parse_origin_form(void)
{
	_uri.scheme = "http://";
	headers_t::iterator		host = _headers.find("Host");
	if (host != _headers.end())
		_uri.authority.assign(host->second);
	std::string::size_type	path_end = _target.find('?');
	if (path_end == std::string::npos)
	{
		_uri.path.assign(_target.begin(), _target.end());
	}
	else
	{
		_uri.path.assign(_target.begin(), _target.begin() + path_end);
		_uri.query.assign(_target.begin() + 1 + path_end, _target.end());
	}
	return (0);
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

static std::string	UpperKey(Request::siterator_t start, Request::siterator_t end)
{
	std::string str(start, end);
	for (std::size_t x = 0; x < str.length(); x++)
	{
		if (x == 0 || str[x - 1] == '-')
		{
			str[x] = toupper(str[x]);
		}
	}
	return str;
}

int	Request::splitHeaders(siterator_t start, siterator_t end)
{
	if (start == end)
		return (0);
	siterator_t		name_end = find(start, end, ':');
	if (name_end == end || name_end == start || std::isspace(*(name_end-1)) == true)
	{
		perror("Request: split_header error (missing name or missing \':\'");
		return (-1);
	}
	siterator_t		value_start = name_end + 1;
	while (value_start != end && *value_start == ' ')
		++value_start;
	std::string		name(start, name_end);
	if (_headers.find("Host") != _headers.end() && name == "Host")
	{
		perror("Request: double Host header fields found");
		return (-1);
	}
	if (end - value_start > HEADER_VALUE_MAX_LENGTH)
	{
		perror("Request: value field too long");
		return (-1);
	}
	_headers[UpperKey(start, name_end)] = std::string(value_start, end);
	return (0);
}

/*
	Fill la map de _headers (key = hd name, value = hd val).
	Return un iterator sur la fin des headers fields, cad:
	- si pas de body, return _rawRqst.end();
	- si body present, return iterator sur son premier caractere.
*/
Request::siterator_t		Request::setHeaders(void)
{
	siterator_t		eof = _rawRqst.end();
	siterator_t		start = _rawRqst.begin() + _rawRqst.find("\r\n") + 2;
	while (start < eof)
	{
		siterator_t		hdr_end = findCRLF(start, eof);
		if (hdr_end == eof) // CRLF pas trouve
		{
			perror("Request: missing CRLF to end message field");
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

const std::string &		Request::getTarget(void) const
{
	return _target;
}

const std::string &		Request::getMethod(void) const
{
	return _method;
}

//TODO: provide a case insensitive comparator to the headers map
const std::string &			Request::getHost(void) const
{
	headers_t::const_iterator	it = _headers.find("Host");
	return it->second;
}

const std::string &		Request::getBody(void) const
{
	return _body;
}

const t_uri&		Request::getUri(void) const
{
	return _uri;
}

std::ostream&	operator<<(std::ostream& o, const Request& me)
{
	o << "Request Line = \'" << me.getRequestLine() << "\'" << std::endl;
	o << "Method = \'" << me.getMethod() << "'\'" << std::endl;
	o << "Target = \'" << me.getTarget() << "\'" << std::endl;
	o << "URI scheme = \'" << me.getUri().scheme << "\'" << std::endl;
	o << "URI auth = \'" << me.getUri().authority << "\'" << std::endl;
	o << "URI path = \'" << me.getUri().path << "\'" << std::endl;
	o << "URI query = \'" << me.getUri().query << "\'" << std::endl;
	return o;
}

void	Request::setLocation(LocationConfig* loc)
{
	this->_loc = loc;
}
void	Request::setConfig(ServerConfig* conf)
{
	this->_conf = conf;
}

ServerConfig*	Request::getConfig(void) const
{
	return _conf;
}

LocationConfig*	Request::getLocation(void) const
{
	return _loc;
}

void	Request::setUploadFile(const std::string& path)
{
	_uploadFileName = path;
}

const std::string &	Request::getUploadFile(void) const
{
	return _uploadFileName;
}
