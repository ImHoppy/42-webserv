#include "CGI.hpp"
#include <cstring> // memcpy()
#include <unistd.h> // fork()
#include <sys/types.h> // fork()
#include <sys/wait.h> // waitpid()
#include <unistd.h> // pipe()
#include <cstdlib> // malloc
#include <stdexcept>
#include <signal.h>

CGI::CGI(void) : _env(), _fileIn(-1), _fileOut(-1), _pid(-1)
{
	initEnv();
}

/* Destructor */
CGI::~CGI(void) {
	CloseFiles();
	if (_pid != -1)
	{
		kill(_pid, SIGKILL);
		waitpid(_pid, NULL, 0);
	}
}

CGI::CGI(const CGI & src)
{
	*this = src;
}

CGI &	CGI::operator=(const CGI & src)
{
	if (&src == this)
		return *this;
	_env = src._env;
	_fileIn = src._fileIn;	
	_fileOut = src._fileOut;	
	_pid = src._pid;	
	return *this;
}

void	CGI::addVarToEnv(const std::string & varval)
{
	_env.push_back(varval);
}

void	CGI::setEnv(std::vector<std::string> & env)
{
	_env = env;
}

void	CGI::initFileIn(const std::string & inputFilename)
{
	_fileIn = open(inputFilename.c_str(), O_RDONLY);
	if (_fileIn == -1)
	{
		Logger::Error("CGI init files read failed errno = %d, %s", errno, strerror(errno));
	}
}

void		CGI::initFileOut(void)
{
	_fileOut = open("/tmp/", O_RDWR | O_TMPFILE );
	if (_fileOut == -1)
	{
		Logger::Error("CGI init files write failed errno = %d, %s", errno, strerror(errno));
	}
}

void	CGI::CloseFiles(void)
{
	if (_fileIn != -1)
		close(_fileIn);
	_fileIn = -1;
	if (_fileOut != -1)
		close(_fileOut);
	_fileOut = -1;
}

/*	1) Init le pipe: _fileOut, pipe side dans lequel le cgi va ecrire sa
	sortie; et _fileIn, depuis lequel le main pourra recuperer les donnees
	ecrites.
	2) Fork() et dup2 out dans _fileOut;
	3) A la sortie du fork on oublis pas de wait (en non bloquant) pr zombie;
*/
int		CGI::launch(const std::string & cgi_cmd, const std::string & script)
{
	_pid = fork();
	if (_pid == -1)
	{
		Logger::Error("Response::CgiGet() fork() failed");
		throw std::runtime_error("Fork() failed");
		return -1;
	}
	if (_pid == 0) // child
	{
		_pid = -1;
		if (_fileIn != -1 && dup2(_fileIn, 0) == -1)
			throw std::runtime_error("CGI dup2 read file failed");
		if (dup2(_fileOut, 1) == -1)
		{
			CloseFiles();
			throw std::runtime_error("CGI dup2 write file failed");
		}
		CloseFiles();
		
		char	**env = new char*[this->_env.size() + 1];
		int	j = 0;
		for (std::vector<std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
			env[j] = new char[i->size() + 1];
			env[j] = strcpy(env[j], (const char*)i->c_str());
			j++;
		}
		env[j] = NULL;
		//char * const * nil = NULL;
		char **argv = new char*[3];
		argv[0] = const_cast<char*>(cgi_cmd.c_str());
		argv[1] = const_cast<char*>(script.c_str());
		argv[2] = NULL;

		Logger::Error("CGI script %s", script.c_str());
		execve(cgi_cmd.c_str(), argv, env);
		Logger::Error("Response::CgiGet() execve() failed");
		for (size_t i = 0; env[i]; i++)
			delete [] env[i];
		delete [] argv;
		delete [] env;
		throw CGI::CGIError();
	}
	return 0;
}

int	CGI::waitCGI(void)
{
	if (_pid == -1) return -1;

	int		status = 0;
	int w = waitpid(_pid, &status, WNOHANG);
	if (w == -1)
	{
		Logger::Error("CGI: waitpid() failed");
		return -1;
	}
	if (w != 0 && WIFEXITED(status))
	{
		Logger::Error("CGI: execve exited %d %d", w,  WEXITSTATUS(status));
		_pid = -1;
		if (WEXITSTATUS(status) == 2)
			return 404;
		return 1;
	}
	if (w != 0 && WIFSIGNALED(status))
	{
		_pid = -1;
		Logger::Error("CGI: execve signaled %d", WTERMSIG(status));
		return -1;
	}
	return 0;
}

void	CGI::initEnv(void)
{
	int i = 0;
	while (environ && environ[i])
	{
		_env.push_back(environ[i]);
		++i;
	}
}

int		CGI::getOutputFile(void) const
{
	return _fileOut;
}
