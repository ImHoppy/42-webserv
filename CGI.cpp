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
	if (_fileIn != -1)
		close(_fileIn);
	if (_fileOut != -1)
		close(_fileIn);
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

int		CGI::initFiles(const std::string & inputFilename)
{
	_fileIn = open(inputFilename.c_str(), O_RDONLY);
	if (_fileIn == -1)
	{
		Logger::Error("CGI init files read failed errno = %d, %s", errno, strerror(errno));
		return (-1);
	}
	_fileOut = open("/tmp/", O_RDWR | O_TMPFILE );
	if (_fileOut == -1)
	{
		Logger::Error("CGI init files write failed errno = %d, %s", errno, strerror(errno));
		return (-1);
	}
	return (0);
}

//TODO: check les malloc returns;
/*	1) Init le pipe: _fileOut, pipe side dans lequel le cgi va ecrire sa
	sortie; et _fileIn, depuis lequel le main pourra recuperer les donnees
	ecrites.
	2) Fork() et dup2 out dans _fileOut;
	3) A la sortie du fork on oublis pas de wait (en non bloquant) pr zombie;
*/
int		CGI::launch(void)
{
	if (_fileIn == -1 || _fileOut == -1)
		throw std::runtime_error("tmp CGI files init needed");
	_pid = fork();
	if (_pid == -1)
	{
		Logger::Error("Response::phpCgiGet() fork() failed");
		close(_fileIn);
		_fileIn = -1;
		close(_fileOut);
		_fileOut = -1;
		return -1;
	}
	if (_pid == 0) // child
	{
		if (dup2(_fileIn, 0) == -1)
		{
			Logger::Error("CGI::dup2 pipeREAD failed");
			close(_fileIn);
			close(_fileOut);
			return -1;
			
		}
//		close(_fileIn);
		if (dup2(_fileOut, 1) == -1)
		{
			Logger::Error("CGI::dup2 pipeWRITE failed");
			close(_fileIn);
			close(_fileOut);
			return -1;
			
		}
//		close(_fileOut);
		char **argv;
		argv = (char **)malloc(sizeof(char *) * 2);
		argv[0] = (char*)malloc(_path.size() + 1);
		argv[0] = (char*)memcpy(argv[0], _path.c_str(), _path.size());
		argv[0][_path.size()] = '\0';
		argv[1] = NULL;
		
		char	**env = new char*[this->_env.size() + 1];
		int	j = 0;
		for (std::vector<std::string>::const_iterator i = this->_env.begin(); i != this->_env.end(); i++) {
			env[j] = new char[i->size() + 1];
			env[j] = strcpy(env[j], (const char*)i->c_str());
//			std::cerr << "Env: " << env[j] << std::endl;
			j++;
		}
		env[j] = NULL;
		if (execve(argv[0], argv, env) == -1)
		{
			Logger::Error("Response::phpCgiGet() execve() failed");
			exit(-1);
		}
		for (size_t i = 0; env[i]; i++)
			delete[] env[i];
		delete[] env;
	}
//	close(_fileIn);
//	_fileIn = -1;
//	close(_fileOut);
//	_fileOut = -1;
	int		status = 0;
	int w = waitpid(_pid, &status, 0);
	if (w == -1)
	{
		Logger::Error("Response::phpCgiGet() waitpid() failed");
		return -1;
	}
	if (WIFEXITED(status))
		Logger::Info("php child exited with %d", WEXITSTATUS(status));
	if (WIFSIGNALED(status))
	{
		Logger::Info("php child signaled with %d", WTERMSIG(status));
		return -1;
	}
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
