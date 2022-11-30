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
		Timeout &	operator=(const Timeout& rhs);
		Timeout &	operator=(const int ms);
		
		void		updateStart(void);
		bool		isTimeOut(void) const;
	private:
		long int	_start;
		long int	_timeout;
};
