/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cdefonte <cdefonte@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/19 11:56:00 by cdefonte          #+#    #+#             */
/*   Updated: 2022/11/01 16:25:26 by cdefonte         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
chacune de ces applications se voit attribuer une adresse unique sur la machine,
codée sur 16 bits: un port (la combinaison adresse IP + port est alors une
adresse unique au monde, elle est appelée socket).
*/

#ifndef MAX_READ
# define MAX_READ 5000
#endif

#include <sstream> // for string stream (int to string)
#include <fstream>
#include "request.hpp"
#include "sockinfo.hpp"
#include "server.hpp"
#include <list>
#include <string>
#include <stdio.h>
#include <errno.h>
#include <iostream> // std
#include <unistd.h> // close
#include <stdlib.h> // exit
#include <poll.h> // poll
#include <sys/types.h>
#include <sys/socket.h>
#include <signal.h>

bool		end_server = false;
void	handler_sigint(int sig)
{
	end_server = true;
	std::cerr << "SIGINT " << sig << std::endl;
}

void	maj_fd_array(struct pollfd *pfds, int *nb_ofds)
{
	for (int i = 0; i < *nb_ofds; ++i)
	{
		if (pfds[i].fd <= 0) // need to get rid of it
		{
			for (int j = i; j < *nb_ofds - 1; ++j)
			{
				pfds[j].fd = pfds[j + 1].fd;
				pfds[j].revents = pfds[j + 1].revents;
			}
			--i;
			--(*nb_ofds);
		}
	}
}

int	do_send(struct pollfd &pollfd)
{
	std::ifstream	htlm_doc("test.html");
	std::string		readout;
	std::string		buff;
	std::string		tmp;
	while (getline(htlm_doc, readout))
	{
		if (readout.size() != 0)
			tmp += readout;
		tmp += "\r\n";
	}
	htlm_doc.close();
	buff = tmp;
	ssize_t	bits_send = send(pollfd.fd, buff.c_str(), buff.size(), 0);
	if (bits_send == -1)
	{
		perror("send failed");
		return (-1);
	}
	else
	{
		std::cout << "Msg send to " << pollfd.fd << " is " << buff.size()<< " bytes and send returned " << bits_send << std::endl;
	}
	return (0);
}

int	do_recv(struct pollfd &pollfd)
{
	char buff[MAX_READ];
	int rc = recv(pollfd.fd, &buff, MAX_READ -1, 0);
	if (rc == -1)
	{
		perror("recv failed");
		return (-1);
	}
	else if (rc == 0 || (pollfd.revents & POLLRDHUP) || (pollfd.revents & POLLHUP))
	{
		std::cout << "Enf of file received from " << pollfd.fd << std::endl;
		close(pollfd.fd);
		pollfd.fd = -1; // set the closing flag to maj pfds array
	}
	else
	{
		buff[rc] = 0;
//		std::cout << "Msg received from " << pollfd.fd << " is: \"" << buff << "\" (" << rc << " bytes)"  << std::endl;
//		std::cout << "\n______	REQUEST OBJCT MAP CONTAINS:" << std::endl; 
		request		rqst(buff);
//		std::cout << "Size map is " << rqst._map.size();
//		std::cout << "_____	END MAP RQST" << std::endl;
		if (pollfd.revents & POLLOUT)
		{
//			if (rqst._map["request_uri"] == "/favicon.ico") // valable que pour Chrome
//			{
//				close(pollfd.fd);
//				pollfd.fd = -1;
//			}
			if (rqst._map["method"] == "GET")
			{
				std::cout << "fd " << pollfd.fd << " POLLOUT GET revent" << std::endl;
				if (do_send(pollfd) == -1)
				{
					return (-1);
				}
			}
		}
	}
	return (0);
}

int		main(int argc, char **argv, char **env)
{
	(void)argc;
	(void)argv;
	(void)env;

	server	myserver;

	/* Create server listening socket */
	/* Create tcp socket */
	struct protoent		*proto;
	int					tcp_lsocket;
	proto = getprotobyname("tcp");
	tcp_lsocket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, proto->p_proto);
	if (tcp_lsocket == -1)
	{
		perror("create lsocket failed");
		return (1);
	}
	int	ok = 1;
//	bool	ok = 1;
	if (setsockopt(tcp_lsocket, SOL_SOCKET, SO_REUSEPORT, &ok, sizeof(ok)) == -1)
	{
		perror("setsockopt failed");
		close(tcp_lsocket);
		return (1);
	}
	/* Bind tcp socket */
	struct sockaddr_in	sin;
	sin.sin_family = AF_INET;
	if (argc == 2)
		sin.sin_port = htons(atoi(argv[1]));
	else
		sin.sin_port = htons(myserver._port);
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(tcp_lsocket, reinterpret_cast<struct sockaddr*>(&sin), sizeof(sin)) == -1)
	{
		perror("bind socket failed");
		close(tcp_lsocket);
		return (1);
	}
	/* Listen socket */
	if (listen(tcp_lsocket, 32) == -1)
	{
		perror("listen failed"); 
		close(tcp_lsocket);
		return (1);
	}
	std::list<sockinfo>		sfd_lst;
	sockinfo	lsock(tcp_lsocket, tcp_lsocket, reinterpret_cast<struct sockaddr*>(&sin), sizeof(sin));
	sfd_lst.push_back(lsock);

	/* Set poll */
	struct pollfd	*pfds = new struct pollfd[200]();
	int				nb_ofds = 1; // only the tcp listening socket for now 
//	int				timeout = 1 * 60 * 1000; // 1 minute
	int				new_fd = -1; // fds created by accept()

	pfds[0].fd = tcp_lsocket;
	pfds[0].events = POLLIN;
	pfds[0].revents = 0;

	int ready;

	while (end_server == false)
	{
		signal(SIGINT, handler_sigint);
		ready = poll(pfds, nb_ofds, -1);
//		if (ready == 0)
//		{
//			perror("Poll timed out\n");
//			break; 
//		}
//		else if (ready == -1)
		if (ready == -1)
		{
			perror("poll error:");
			break;
		}
		for (int i = 0; i < nb_ofds; ++i)
		{
			if (pfds[i].revents & POLLERR)
			{
				end_server = true;
				std::cerr << "Error: pfds[ " << i << "] revents is " << pfds[i].revents << std::endl;
				break;
			}
			if (pfds[i].fd == tcp_lsocket) // si le listen socket is readable
			{
//				std::cerr << "Listening socket is readable" << std::endl;
				do
				{
					struct sockaddr		addr;
					memset(&addr, 0, sizeof(struct sockaddr));
					socklen_t			len = sizeof(struct sockaddr_in);
					new_fd = accept(tcp_lsocket, &addr, &len); 
					if (new_fd < 0)
					{
//						std::cerr << "new fd is negative " << std::endl;
						/* If the socket is marked nonblocking and no pending 
						connections are present on the queue, accept() fails with 
						the error EAGAIN or EWOULDBLOCK. */
						if (errno != EAGAIN && errno != EWOULDBLOCK)
						{
							perror("accept() failed");
							end_server = true;
							break;
						}
					}
					else
					{
						sfd_lst.push_back(sockinfo(new_fd, tcp_lsocket, &addr, len));
						std::cout << "New INCOMING CONNECTION with fd " << new_fd;
						std::cout << " addr is " << ntohl(sfd_lst.back().get_in_addr());
						std::cout << " port is " << ntohs(sfd_lst.back().get_port()) << std::endl;
						sfd_lst.back().print_addr();

						pfds[nb_ofds].fd = new_fd;
						pfds[nb_ofds].events = POLLIN | POLLHUP | POLLPRI | POLLOUT;
						pfds[nb_ofds].revents = 0;
						++nb_ofds;

					}
				} while (new_fd != -1); // end accept (non blocking) all connections
			} // end special case for listening socket
			else // is not the listening socket
			{
				if (pfds[i].revents & POLLERR)
				{
					std::cout << "fd " << pfds[i].fd << " revents POLLERR" << std::endl;
				}
				else if (pfds[i].revents & POLLPRI)
				{
					std::cout << "fd " << pfds[i].fd << " POLLPRI revents" << std::endl;
				}
				else if (pfds[i].revents & POLLRDHUP)
				{
					std::cout << "fd " << pfds[i].fd << " POLLRDHUP revents" << std::endl;
				}
				else if (pfds[i].revents & POLLHUP)
				{
					std::cout << "fd " << pfds[i].fd << " POLLHUP revents" << std::endl;
				}
				if (pfds[i].revents & POLLIN)
				{
					std::cout << "fd " << pfds[i].fd << " POLLIN revent" << std::endl;
					if (do_recv(pfds[i]) == -1)
					{
						end_server = true;
						break ;
					}
				}
			}
		} // end loop check revents on all fds

		maj_fd_array(pfds, &nb_ofds);
		if (nb_ofds == 0)
		{
			end_server = true;
			std::cerr << "No more socket fds open" << std::endl;
		}
	} // end while end_server == false
	
	for (int i = 0; i < nb_ofds; ++i)
	{
		std::cout << i << std::endl;
		if (pfds[i].fd > 0 && pfds[i].fd != tcp_lsocket)
		{
			if (close(pfds[i].fd) == -1)
				perror("closing client socket fd failed");
		}
	}
	if (close(pfds[0].fd) == -1)
		perror("closing tcp listening socket failed");
	else
		std::cerr << "Server socket closed successfuly" << std::endl;
	delete [] pfds;
	return (0);
}
