#pragma once

#include <string>
#include <vector>

#include <cstdio> // FILE*
#include <errno.h> // errno
#include <string.h> // strerror
#include <sys/types.h> // open
#include <sys/stat.h> // open
#include <fcntl.h> // open


#include "Logger.hpp"

#ifndef BUFFSIZE
# define BUFFSIZE 1500
#endif

class CGI {
	private:
		std::vector<std::string>	_env;
		int							_fileIn;
		int							_fileOut;
		pid_t						_pid;

	public:
	enum StatusCGI {
		NOT_CGI = 0,
		READY_TO_LAUNCH,
		IN_PROGRESS,
		FINISH,
		FAILED
	};
		class CGIError
		{
			public:
			CGIError(){};
			CGIError(const CGIError&){};
			CGIError& operator=(const CGIError&){return *this;};
			virtual ~CGIError(){};
			virtual const char* what() const { return "CGI Error"; };
		}; // end execveError
		CGI(void);
		~CGI(void);
		CGI &	operator=(const CGI & src);
		CGI(const CGI & src);
		/* GET */
		const std::vector<std::string> &	getEnv(void) const;
		int									getOutputFile(void) const;
		/* SET */
		void	setEnv(std::vector<std::string> & env);
		void	initEnv(void);
		int		initFiles(const std::string & inputFilename);
		void		initFileOut(void);
		void		initFileIn(const std::string & inputFilename);
		void	addVarToEnv(const std::string & varval);

		int		launch(const std::string & gci_cmd, const std::string & script);
		void	CloseFiles(void);
		int		waitCGI(void);

};
