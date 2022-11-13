#include "Response.hpp"

/* Default constructor */
Response::Response(void) : _config(), _rqst(), _headers(), _response() {}

/* Destructor */
Response::~Response(void) {}

/* Copy constructor */
Response::Response(const Response& src) : _config(src._config), _rqst(src._rqst),
	_headers(src._headers), _response(src._response) {}

/* Assignement operator */
Response &	Response::operator=(const Response& src)
{
	if (this != &src)
	{
		_config = src._config;
		_rqst = src._rqst;
		_headers = src._headers;
		_response = src._response;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, Request* request) :
	_config(config),
	_rqst(request),
	_headers(),
	_response()
{
	
}

std::ostream&	operator<<(std::ostream& o, const Request& me);
{
	
	return o;
}
