#pragma once

#include <string>

class SocketHandler {
	protected:
	std::string	_type;
	public:
	SocketHandler(std::string const & type) : _type(type) {};
	virtual ~SocketHandler() {};

	virtual std::string const & getType() const = 0;
};
