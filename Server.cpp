#include "Server.hpp"

#include <netdb.h> // getaddrinfo
#include <list>

/* Return true if one of the _configs has the same HOST::PORT pairs. */
bool	Server::isSameHostPort(int32_t host, int16_t port) const
{
	for (std::vector<ServerConfig>::const_iterator conf_it = _configs.begin(); conf_it != _configs.end(); ++conf_it)
	{
		if (conf_it->getPort() == port && (conf_it->getHost() == host || host == 0 || conf_it->getHost() == 0))
			return true;
	}
	return false;
}

/* Default Constructor */
Server::Server(void) : SocketHandler("Server"), _socket(-1), _configs(),
	_clients(), _epollInstance(-1)
{
	#ifdef CONSTRUC
	std::cerr << "Server Default constructor" << std::endl;
	#endif
}

/* Copy Constructor */
Server::Server(Server const & other) : SocketHandler("Server"), _socket(other._socket), 
	_configs(other._configs), _clients(other._clients), 
_epollInstance(other._epollInstance)
{
	#ifdef CONSTRUC
	std::cerr << "Server Copy constructor" << std::endl;
	#endif
}

/* Assignement operator (should be private) */
Server &	Server::operator=(Server const & other)
{
	#ifdef CONSTRUC
	std::cerr << "Server Assignement operator" << std::endl;
	#endif
	if (this != &other) {
		_socket = other._socket;
		_configs = other._configs;
		_clients = other._clients;
	}
	return *this;
}

/* Destructor */
Server::~Server(void)
{
	#ifdef CONSTRUC
	std::cerr << "Server Destructor" << std::endl;
	#endif
	for (set_client::iterator	it = _clients.begin(); it != _clients.end(); ++it)
	{
		delete *it;
	}
}

std::vector<ServerConfig> const &	Server::getConf(void) const { return _configs; }
socket_t	Server::getSocket(void) const { return _socket; }
std::string const &	Server::getType(void) const { return _type; }

void	Server::addConfig(ServerConfig const & config)
{
	_configs.push_back(config);
}

void	Server::setEpollInstance(int inst)
{
	_epollInstance = inst;
}

/* Cherche parmis les configs de ce server (en gros parmis ses virtuals servers)
lequel a le meme server_name que le Host header de la requete. 
Return:
	1) Si aucun Host header n'est present, on return le VServer par default (en
	gros le premier). Celui-ci se chargera de respond 400 bad request.
	2) Fait la liste de tous les host:port qui matchent l'ip requested
	3) Check les server_names pour chaque match, si un est ok return celui-ci.
	Si aucun VServer name ne correpond, le default VServer (le premier match)
	se chargera de servir la rqst.
*/
ServerConfig*	Server::getConfigForRequest(Request* rqst, int client_socket)
{
	std::string		host_header(rqst->getValForHdr("Host"), 0, rqst->getValForHdr("Host").find(':'));
	if (host_header.empty()) // Bad request
		return &_configs[0];

	// Determine the actual interface address from server socket
	struct sockaddr_in server_addr = {};
	socklen_t server_addr_len = sizeof(server_addr);
	if (getsockname(client_socket, (struct sockaddr *)&server_addr, &server_addr_len) < 0)
	{
		Logger::Error("getsockname() failed");
		for (std::vector<ServerConfig>::iterator it = _configs.begin(); it != _configs.end(); ++it)
		{
			if (it->getHost() == 0)
				return &(*it);
		}
		return &_configs[0];
	}

	int32_t	ip = ntohl(server_addr.sin_addr.s_addr);

	/* On fait une liste des tous les blocks qui matchent l'ip requested */
	std::list<ServerConfig*>	matchs;
	for (std::vector<ServerConfig>::iterator it = _configs.begin(); it != _configs.end(); ++it)
	{
		if (ip == it->getHost())
			matchs.push_back(&(*it));
	}
	/* Si matchs is empty c'est qu'il y a forcement un 0.0.0.0, a retourner. */
	for (std::vector<ServerConfig>::iterator it = _configs.begin(); it != _configs.end(); ++it)
	{
		if (it->getHost() == 0)
			matchs.push_back(&(*it));
	}
	/* Dans la liste des match ip:port on check si l'un d'eux a un servername qui
	correspond au Host header. Si aucun, on return le premier de la liste. */
	for (std::list<ServerConfig*>::iterator it = matchs.begin(); it != matchs.end(); ++it)
	{
		std::vector<std::string>	names = (*it)->getServerNames();
		if ((*it)->hasSeverName(host_header))
			return *it;
	}
	return matchs.front();
}

/* Terminates connection with the 'client': deletes it frome the epoll fds,
close the connected socket, delete its memory and removes it from the set 
of Clients.*/
void	Server::removeClient(Client* client)
{
	socket_t socket = client->getSocket();
	if (epoll_ctl(_epollInstance, EPOLL_CTL_DEL, socket, NULL) < 0) {
		throw std::runtime_error("epoll_ctl DEL after EOF failed");
	}
	close(socket);
	set_client::iterator	it = _clients.find(client);
	if (it != _clients.end())
	{
		delete *it;
		_clients.erase(it);
	}
}

int	Server::BindableHost()
{
	for (std::vector<ServerConfig>::const_iterator conf_it = _configs.begin(); conf_it != _configs.end(); ++conf_it)
	{
		if (conf_it->getHost() == 0)
			return 0;
	}
	return _configs[0].getHost();
}

/* Create, bind, and set in listen state its _socket. */
int		Server::InitServer(void)
{
	struct sockaddr_in servaddr;

	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		Logger::Error("Server: socket() failed: %s", strerror(errno));
		return (-1);
	}
	int on = 1;
	if (setsockopt(_socket, SOL_SOCKET,  SO_REUSEADDR, &on, sizeof(int)) < 0)
	{
		Logger::Error("Server: setsockopt() failed: %s", strerror(errno));
		close(_socket);
		_socket = -1;
		return (-1);
	}
	std::memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(BindableHost());
	servaddr.sin_port = htons(_configs[0].getPort());
	if (bind(_socket, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0)
	{
		Logger::Error("Server: bind() failed: %s", strerror(errno));
		return (-1);
	}
	if (listen(_socket, std::numeric_limits<short>::max()/8) < 0)
	{
		perror("Server: Listen failed");
		return (-1);
	}
	return (0);
}

/* Create a new client socket with accept, and create a new Client instance with it.
Then add it with its pair client socket to the _clients set. */
socket_t	 Server::AcceptNewClient(void)
{
	struct epoll_event event;
	struct sockaddr_in client_addr;
	socklen_t client_addr_len = sizeof(client_addr);
	socket_t client_socket = accept(_socket, (struct sockaddr *)&client_addr, &client_addr_len);
	if (client_socket < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
	{
		throw std::runtime_error("accept() failed");
	}
	else if (client_socket < 0)
	{
		perror("accept() failed");
		return (-1);
	}
	Client * client = new Client(client_socket, this);
	event.data.ptr = client;
	event.events = EPOLLIN;
	if (epoll_ctl(_epollInstance, EPOLL_CTL_ADD, client_socket, &event) < 0) {
		throw std::runtime_error("epoll_ctl failed");
	}
	_clients.insert(client);
	Logger::Info("Server %d accepted new client on fd %d", _socket, client_socket);
	return (client_socket);
}

void	Server::readyToRead(Client* client)
{
	struct epoll_event event;
	event.data.ptr = client;
	event.events = EPOLLOUT | EPOLLIN;
	if (epoll_ctl(_epollInstance, EPOLL_CTL_MOD, client->getSocket(), &event) < 0) {
		throw std::runtime_error("epoll_ctl OUT failed");
	}
}

void	Server::checkTimeout(void)
{
	std::vector<Client *> timeoutClients;

	for (set_client::iterator it = _clients.begin(); it != _clients.end(); ++it)
	{
		Client* client = *it;
		if (client && client->hasTimeout())
		{
			try
			{
				this->respond(client);
			}
			catch(const std::exception& e)
			{
				Logger::Error("Problem client response: %s", e.what());
			}
			timeoutClients.push_back(client);
		}
	}
	for (std::vector<Client *>::iterator itClient = timeoutClients.begin(); itClient != timeoutClients.end(); itClient++)
	{
		Client* client = *itClient;
		Logger::Info("Client %d timed out", client->getSocket());
		this->removeClient(client);
	}
}

/*	1) Find la bonne config grace au host header
	2) Find la bonne location grace a l'URL
	3) Cree une Resposne avec ces donees
	4) Send la Response
*/
void	Server::respond(Client* client)
{
	int	bytes = 0;

	if (client == NULL) return;

	Response	*rep = client->getResponse();
	if (rep == NULL)
	{
		Request*	rqst = client->getRequest();
		if (rqst == NULL)
			return ;
		rep = new Response(rqst, client);
		client->setResponse(rep);

		if (rep->cgiStatus == CGI::NOT_CGI)
		{
			rep->generateResponse();
			bytes = send(client->getSocket(), rep->getResponse().c_str(), rep->getResponse().size(), 0);
			Logger::Info("%d %s", rep->getCode().first, rep->getCode().second.c_str());
		}
	}
	else if (rep->cgiStatus == CGI::NOT_CGI) // Chunk get methods
	{
		Response::ReadData const & data = rep->getReadData();
		if (data.buffer == NULL) return ;

		rep->tryFile();

		bytes = send(client->getSocket(), data.buffer, data.read_bytes, 0);
	}
	if (client->hasTimeout())
	{
		rep->cgiStatus = CGI::FINISH;
	}
	if (rep->cgiStatus == CGI::READY_TO_LAUNCH)
	{
		rep->doCGI();
	}
	if (rep->cgiStatus == CGI::IN_PROGRESS)
	{
		rep->waitCGI();
		if (rep->cgiStatus == CGI::IN_PROGRESS)
			return ;
	}
	if (client->hasTimeout() || rep->cgiStatus == CGI::FINISH)
	{
		rep->generateResponse();
		bytes = send(client->getSocket(), rep->getResponse().c_str(), rep->getResponse().size(), 0);
		Logger::Info("CGI %d %s", rep->getCode().first, rep->getCode().second.c_str());
	}

	if (rep->cgiStatus == CGI::FINISH || rep->getReadData().status == Response::NONE)
	{
		client->popOutRequest();
		client->popOutResponse();
	}
	if (bytes == 0 || bytes == -1)
	{
		throw std::runtime_error("send failed");
	}
}
