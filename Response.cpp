#include "Response.hpp"

/* Default constructor */
Response::Response(void) : _config(), _rqst(), _response("DEFAULT RESPONSE") {}

/* Destructor */
Response::~Response(void) {}

/* Copy constructor */
Response::Response(const Response& src) : _config(src._config), _rqst(src._rqst),
	_response(src._response) {}

/* Assignement operator */
Response &	Response::operator=(const Response& src)
{
	if (this != &src)
	{
		_config = src._config;
		_rqst = src._rqst;
		_response = src._response;
	}
	return *this;
}

/* Parametric constructor */
Response::Response(ServerConfig* config, LocationConfig* loc, Request* request) :
	_config(config),
	_location(loc),
	_rqst(request),
	_response("DEFAULT RESPONSE")
{
}

std::string	Response::getResponse(void) const
{
	return _response;
}

std::ostream&	operator<<(std::ostream& o, const Response& me)
{
	o << me.getResponse();
	return o;
}
