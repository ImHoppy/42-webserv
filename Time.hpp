#pragma once

namespace Date
{
	long int	Now(void);
}

class Timeout
{
	public:
		Timeout(void);
		Timeout(int ms);
		Timeout(const Timeout& src);
		~Timeout(void);
		Timeout & operator=(const Timeout& rhs);
		
		void		updateStart(void);
		void		setTimeOut(int ms);
		bool		isTimeOut(void);
	private:
		long int	_start;
		long int	_timeout;
};
