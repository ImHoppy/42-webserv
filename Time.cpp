#include "Time.hpp"
#include "Logger.hpp"
#include <sys/time.h>

long int	Date::Now(void)
{
	struct timeval tp;
	gettimeofday(&tp, NULL);
	long int ms = tp.tv_sec * 1000 + tp.tv_usec / 1000;
	return (ms);
}



Timeout::Timeout(void)
{
	_start = Date::Now();
	_timeout = 0;
}

Timeout::Timeout(const Timeout& src)
{
	*this = src;
}

Timeout::~Timeout(void)
{}

Timeout & Timeout::operator=(const Timeout& rhs)
{
	if (this != &rhs)
	{
		_start = rhs._start;
		_timeout = rhs._timeout;
	}
	return (*this);
}

Timeout & Timeout::operator=(const int ms)
{
	_start = Date::Now();
	_timeout = ms;
	return (*this);
}


Timeout::Timeout(int ms)
{
	_start = Date::Now();
	_timeout = ms;
}

void	Timeout::updateStart(void)
{
	_start = Date::Now();
}

bool	Timeout::isTimeOut(void) const
{
	long int now = Date::Now();
	return (now - _start > _timeout);
}
