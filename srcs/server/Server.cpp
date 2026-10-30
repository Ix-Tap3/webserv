/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 21:06:27 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/23 13:09:27 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <fcntl.h>
#include <netdb.h>

Server::Server(const ServerConfig &config): _config(config)
{
}

Server::~Server()
{
}

// === SETUP PART === //
bool	Server::setup()
{
	for (size_t i = 0; i < this->_config.ports.size(); i++)
	{
		if (!setupListener(this->_config.ports[i]))
			return (false);
	}
	return (true);
}

bool	Server::setupListener(int port)
{
	struct addrinfo hints = {};
	struct addrinfo *res = NULL;
	std::string		portStr = Utils::intToString(port);

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	int status = getaddrinfo(this->_config.ip.c_str(), portStr.c_str(), &hints, &res);
	if (status != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return (false);
	}

	if (!setupSocket(res))
		return (false);

	int opt = 1;
	if (setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		close(this->_socketFd);
		freeaddrinfo(res);
		return (false);
	}

	if (!setupBind(res))
	{
		std::cerr << "Cannot listen on " << this->_config.ip << ":" << port << std::endl;
		return (false);
	}

	if (!setupListen(res))
		return (false);

	this->_pollFds.push_back(setupServerPoll());
	this->_socketFds.push_back(this->_socketFd);

	freeaddrinfo(res);
	std::cout << "Listening on " << this->_config.ip << ":" << port << std::endl;
	return (true);
}

bool			Server::setupSocket(struct addrinfo *res)
{
	this->_socketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->_socketFd == -1)
	{
		std::cerr << "socket: " << std::strerror(errno) << std::endl;
		freeaddrinfo(res);
		return (false);
	}
	if (fcntl(this->_socketFd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(this->_socketFd);
		freeaddrinfo(res);
		return (false);
	}
	return (true);
}

bool			Server::setupBind(struct addrinfo *res)
{
	int res_bind = bind(this->_socketFd, res->ai_addr, res->ai_addrlen);
	if (res_bind == -1)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close(this->_socketFd);
		freeaddrinfo(res);
		return (false);
	}
	return (true);
}

bool			Server::setupListen(struct addrinfo *res)
{
	int res_listen = listen(this->_socketFd, 10);
	if (res_listen == -1)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		return (false);
	}
	return (true);
}

struct pollfd	Server::setupServerPoll()
{
	struct pollfd server_poll;
	server_poll.fd = this->_socketFd;
	server_poll.events = POLLIN;
	server_poll.revents = 0;
	return (server_poll);
}


// === RUN PART === //


bool	Server::run()
{
	while (true)
	{
		int ret = poll(&_pollFds[0], this->_pollFds.size(), 1000);

		if (ret == -1)
		{
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			for (std::vector<struct pollfd>::iterator it = this->_pollFds.begin(); it != this->_pollFds.end(); it++)
				addFdToCloseList(it->fd);
			clearFds();
			return (false);
		}
		for (size_t i = this->_pollFds.size(); i-- > 0; )
		{
			if (isInFdCloseList(this->_pollFds[i].fd))
				continue ;
			// If server fd
			if (isServerSocket(this->_pollFds[i].fd))
			{
				if (!watchServerSocket(i))
				{
					return (false);
				}
			}
			// If client fd
			else if (Server::isClientFd(this->_pollFds[i].fd))
			{
				if (!watchClientsSocket(i))
				{
					return (false);
				}
			}
			else
			{
				addFdToCloseList(this->_pollFds[i].fd);
				std::cout << "Unknown fd encouter in run() fuction" << std::endl;
			}
			// If CGI fd
		}
		handleInactiveClient();
		clearFds();
	}
	return (true);
}

// Server Socket //
bool	Server::isServerSocket(int fd) const
{
	for (size_t i = 0; i < this->_socketFds.size(); i++)
	{
		if (this->_socketFds[i] == fd)
			return (true);
	}
	return (false);
}

bool	Server::watchServerSocket(int index)
{
	if (this->_pollFds[index].revents & POLLIN)
	{
		if (!acceptNewClient(this->_pollFds[index].fd))
			return (false);
	}
	return (true);
}

bool	Server::acceptNewClient(int serverFd)
{
	int	client_fd = accept(serverFd, NULL, NULL);

	if (client_fd == -1)
	{
		//std::cerr << "accept: " << std::strerror(errno) << std::endl;
		//close(this->_socketFd);
		//return (false);
		return (true);
	}
	if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
	{
		close(client_fd);
		return (true);
	}
	struct pollfd newClient = {client_fd, POLLIN, 0};

	this->_pollFds.push_back(newClient);
	this->_clients.insert(std::make_pair(client_fd, Client(client_fd, &this->_config)));

	std::cout << "Client " << client_fd << " connected!" << std::endl;
	return (true);
}

// Clients Socket //
bool	Server::watchClientsSocket(int index)
{
	int		client_fd = this->_pollFds[index].fd;
	Client	*client = &this->_clients[client_fd];

	if (this->_pollFds[index].revents & (POLLERR | POLLHUP | POLLNVAL))
	{
		addFdToCloseList(this->_pollFds[index].fd);
		return (true);
	}
	if (this->_pollFds[index].revents & POLLIN)
	{
		if (!this->clientPOLLIN(client, client_fd, index))
			return (false);
	}
	else if (this->_pollFds[index].revents & POLLOUT)
	{
		if (!this->clientPOLLOUT(client, client_fd, index))
			return (false);
	}
	return (true);
}

bool	Server::clientPOLLIN(Client *client, int clientFd, int index)
{
	char	buff[100];

	int	res = recv(clientFd, buff, sizeof(buff), 0);
	// std::cout << "Data received: " << std::string(buff, res) << std::endl; // test purpose
	// Error
	if (res <= 0)
	{
		std::cout << "Client closed connection" << std::endl; // test purpose
		this->addFdToCloseList(this->_pollFds[index].fd);
		return (true);
	}
	// No problem
	client->setActivityTime();
	client->appendReceivedData(buff, res);
	if (client->getContentLength() == -1 && client->hasCompleteHeaders())
	{
		if (!client->stashHeaders())
		{
			this->_pollFds[index].events = POLLOUT;
			return (true);
		}
	}
	if (client->isReadingChunks())
	{
		if (client->processChunkedBody())
		{
			if (client->getConnectionState())
				this->_pollFds[index].events = POLLOUT | POLLIN;
			else
				this->_pollFds[index].events = POLLOUT;
		}
		return (true);
	}
	if (client->getContentLength() != -1
		&& client->getContentLength() <= client->getNbBodyByte())
	{
		client->stashBody();
		this->_pollFds[index].events = POLLOUT | POLLIN;
	}
	return (true);
}

bool	Server::clientPOLLOUT(Client *client, int clientFd, int index)
{
	if (!client->hasSomethingToSend())
	{
		if (!client->getConnectionState())
			this->addFdToCloseList(this->_pollFds[index].fd);
		else
			this->_pollFds[index].events = POLLIN;
		return (true);
	}

	const std::string &response = client->getSendBuffer();;
	ssize_t	byte_send = send(clientFd, response.c_str(), response.length(), 0);
	if (byte_send <= 0)
	{
		this->addFdToCloseList(this->_pollFds[index].fd);
		return (true);
	}
	
	client->setActivityTime();
	client->removeReponseSend(byte_send);

	return (true);
}

// --- Utils
void	Server::addFdToCloseList( int fd )
{
	for (std::vector<int>::iterator it = this->_fdsToClose.begin(); it != this->_fdsToClose.end(); it++)
		if (*it == fd)
			return ;
	this->_fdsToClose.push_back(fd);
}

void	Server::clearFds( void )
{
	if (this->_fdsToClose.empty())
		return ;

	for (std::vector<int>::iterator it = this->_fdsToClose.begin(); it != this->_fdsToClose.end(); it++)
	{
		close(*it);
		if (isClientFd(*it))
		{
			std::cout << "Client " << *it << " disconnected!" << std::endl;
			this->_clients.erase(*it);
		}
	}

	rebuildPollFds();

	this->_fdsToClose.clear();
}

bool	Server::isClientFd( int fd ) const
{
	if (this->_clients.count(fd) > 0)
		return true;
	return false;
}

bool	Server::isInFdCloseList( int fd ) const
{
	for (std::vector<int>::const_iterator it = this->_fdsToClose.begin(); it != this->_fdsToClose.end(); it++)
		if (*it == fd)
			return true;

	return false;
}

void	Server::rebuildPollFds( void )
{
	std::vector<struct pollfd>	newPollFds;

	for (std::vector<struct pollfd>::iterator it = this->_pollFds.begin(); it != this->_pollFds.end(); it++)
		if (!isInFdCloseList(it->fd))
			newPollFds.push_back(*it);

	this->_pollFds = newPollFds;
}

void	Server::handleInactiveClient( void )
{
	time_t	now = std::time(NULL);
	double	limit = 60;

	if (MAX_TIMEOUT > 0 && MAX_TIMEOUT <= 60)
		limit = static_cast<double>(MAX_TIMEOUT);

	for (std::map<int, Client>::iterator it = this->_clients.begin(); it != this->_clients.end(); it++)
	{
		if (std::difftime(now, it->second.getLastActivityTime()) > limit)
		{
			if (it->second.hasPendingRequest())
			{
				if (it->second.hasSomethingToSend())
				{
					addFdToCloseList(it->first);
					continue ;
				}
				int	currentIndex = getClientPosByFd(it->first);

				if (currentIndex == -1)
				{
					addFdToCloseList(it->first);
					continue ;
				}
				this->_pollFds[currentIndex].events = POLLOUT;
				it->second.setActivityTime();
				it->second.prepareErrorResponse(408);
			}
			else
				addFdToCloseList(it->first);
		}
	}
}

int	Server::getClientPosByFd( int fd ) const
{
	for (std::size_t i = 0; i < this->_pollFds.size(); i++)
		if (this->_pollFds[i].fd == fd)
			return static_cast<int>(i);
	return -1;
}
