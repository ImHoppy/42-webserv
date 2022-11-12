#pragma once

#include <string>

class Base {
	protected:
	std::string	_type;
	public:
	Base(std::string const & type) : _type(type) {};
	virtual ~Base() {};

	virtual std::string const & getType() const = 0;
};