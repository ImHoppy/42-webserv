#include "CGI.hpp"
#include <cstring> // memcpy()
#include <unistd.h> // fork()
#include <sys/types.h> // fork()
#include <sys/wait.h> // waitpid()
#include <unistd.h> // pipe()
#include <cstdlib> // malloc

CGI::CGI(void) : _env(), _fileIn(-1), _fileOut(-1), _pid(-1),
	_path("/usr/bin/php-cgi")
{
	initEnv();
}

/* Destructor */
CGI::~CGI(void) {
	Logger::Error("DESTRUCOTR CGI");
	CloseFiles();
}

CGI::CGI(const CGI & src) :
	_env(src._env),
	_fileIn(-1),
	_fileOut(-1),
	_pid(-1),
	_path("/usr/bin/php-cgi")
{
}

CGI &	CGI::operator=(const CGI & src)
{
	_env = src._env;
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
int		CGI::launch(void)
{
	_pid = fork();
	if (_pid == -1)
	{
		Logger::Error("Response::phpCgiGet() fork() failed");
		throw std::runtime_error("Fork() failed");
		return -1;
	}
	if (_pid == 0) // child
	{
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
		char * const * nil = NULL;	
		execve("/usr/bin/php-cgi", nil, env);
		Logger::Error("Response::phpCgiGet() execve() failed");
		for (size_t i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
	else
	{
		int		status = 0;
		int w = waitpid(_pid, &status, 0);
		if (w == -1)
		{
			Logger::Error("Response::phpCgiGet() waitpid() failed");
			return -1;
		}
		if (WIFEXITED(status))
		{
			Logger::Info("php child exited with %d", WEXITSTATUS(status));
			return -1;
		}
		if (WIFSIGNALED(status))
		{
			Logger::Info("php child signaled with %d", WTERMSIG(status));
			return -1;
		}

	}
	CloseFiles();
	if (_pid == 0)
		throw CGI::CGIError();
	return _pid;
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
