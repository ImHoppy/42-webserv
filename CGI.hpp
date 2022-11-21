#ifndef CGI_HPP
# define CGI_HPP

# include <string>
# include <vector>
# include <unistd.h> // fork()
# include <sys/types.h> // fork()
# include <sys/wait.h> // waitpid()
# include <cstring> // memcpy()
# include <unistd.h> // pipe()
# include <cstdlib> // malloc

# include "Logger.hpp"

# ifndef BUFFSIZE
#  define BUFFSIZE 1500
# endif

class CGI {
	private:
		std::vector<std::string>		_env;
		int								_pipefdRead;
		int								_pipefdWrite;
		pid_t							_pid;
		std::string						_path;
//		int								_readFrom; // useless for now mais je pense qu'il faut lui mettre le body de la request dedans si c'est du post par exemple?
	
	public:
		CGI(void);
		~CGI(void);
		CGI &	operator=(const CGI & src);
		CGI(const CGI & src);
		/* GET */
		const std::vector<std::string> &	getEnv(void) const;
		int									getReadPipe(void) const;
		/* SET */
		void	setEnv(std::vector<std::string> & env);
		void	initEnv(void);
		int		initPipe(void);
		void	addVarToEnv(const std::string & varval);

		int		launch(void);

};

#endif

