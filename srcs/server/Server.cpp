/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 21:06:27 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/18 17:01:50 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>
#include <fcntl.h>
#include <netdb.h>

Server::Server()
{
}

Server::~Server()
{
}

// === SETUP PART === //
bool	Server::setup()
{
	struct addrinfo hints = {};
	struct addrinfo *res = NULL;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	int status = getaddrinfo("127.0.0.1", "8080", &hints, &res);
	if (status != 0)
	{
		std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
		return (false);
	}

	if (!setupSocket(res))
		return (false);
	std::cout << "Socket successful" << std::endl;

	int opt = 1;
	if (setsockopt(this->_socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
	{
		perror("setsockopt");
		return (false);
	}

	if (!setupBind(res))
		return (false);
	std::cout << "Bind successful" << std::endl;

	if (!setupListen(res))
		return (false);
	std::cout << "Listen successful" << std::endl;

	
	this->_pollFds.push_back(setupServerPoll());

	freeaddrinfo(res);
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
		int ret = poll(&_pollFds[0], this->_pollFds.size(), -1);

		if (ret == -1)
		{
			std::cerr << "poll: " << std::strerror(errno) << std::endl;
			// Add a function to close all the fds
			return (false);
		}

		for (size_t i = 0; i < this->_pollFds.size(); i++)
		{
			// If server fd
			if (this->_pollFds[i].fd == this->_socketFd)
			{
				if (!watchServerSocket(i))
				{
					return (false);
				}
			}
			// If client fd
			else 
			{
				if (!watchClientsSocket(i))
				{
					return (false);
				}
			}
		}
	}
	return (true);
}

// Server Socket //
bool	Server::watchServerSocket(int index)
{
	if (this->_pollFds[index].revents & POLLIN)
	{
		if (!acceptNewClient())
			return (false);
	}
	return (true);
}

bool	Server::acceptNewClient()
{
	int	client_fd = accept(this->_socketFd, NULL, NULL);

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
	this->_clients.insert(std::make_pair(client_fd, Client(client_fd)));

	std::cout << "Client connected!" << std::endl;
	return (true);
}

// Clients Socket //
bool	Server::watchClientsSocket(int index)
{
	int		client_fd = this->_pollFds[index].fd;
	Client	*client = &this->_clients[client_fd];

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
		std::cout << "Client " << clientFd << " disconnected" << std::endl; // test purpose
		this->removeClient(index);
		return (true);
	}
	// No problem
	client->appendReceivedData(buff, res);
	if (client->hasCompleteHeaders())
	{
		// We received all of the HTTP header
		// that not mean that we received all, there's maybe a body so if we read enougth bytes
		// (number of bytes are indicated by Content-Length)
		if (!client->stashHeaders())
		{
			this->_pollFds[index].events = POLLOUT;
			return (true);
		}
	}
	if (client->getContentLength() <= client->getNbBodyByte())
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
			this->removeClient(index);
		else
			this->_pollFds[index].events = POLLIN;
		return (true);
	}

	const std::string &response = client->getSendBuffer();;
	ssize_t	byte_send = send(clientFd, response.c_str(), response.length(), 0);
	if (byte_send <= 0)
	{
		this->removeClient(index);
		return (true);
	}
	
	client->removeReponseSend(byte_send);

	return (true);
}

void	Server::removeClient(int index)
{
	int	fd = this->_pollFds[index].fd;

	close(fd);
	this->_clients.erase(fd);
	this->_pollFds.erase(this->_pollFds.begin() + index);
}
