#ifndef CGI_HPP
# define CGI_HPP

#include <string>
#include <vector>
#include "Request.hpp"

class CGI {
	private:
		std::vector<std::string>		_env;
	
	public:
		CGI(void);
		~CGI(void);
		CGI &	operator=(const CGI & src);
		CGI(const CGI & src);
		const std::vector<std::string> &	getEnv(void) const;

		void	setEnv(const Request & rqst);
};

#endif

