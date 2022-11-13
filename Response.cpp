#include "Response.hpp"

/* Default constructor */
Response::Response(void) : _config(), _rqst(), _headers(), _response("DEFAULT RESPONSE") {}

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

//bool	Response::requestedMethodIsAllowed(void) const
//{
//	if (_rqst.getMethod() == "GET" && _location
//
//}

/* Parametric constructor */
Response::Response(ServerConfig* config, Request* request) :
	_config(config),
	_rqst(request),
	_headers(),
	_response("DEFAULT RESPONSE")
{
//	if (requestedMethodIsAllowed() == false)
//	{
//		_response = "HTTP/1.1 405 Method Not Allowed";
//		//TODO: check if error_page de 405 et serv it
//	}

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
