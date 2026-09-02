/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 21:06:27 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/02 17:26:34 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

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
		std::cerr << "accept: " << std::strerror(errno) << std::endl;
		close(this->_socketFd);
		return (false);
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
	// Error
	if (res == -1)
	{
		std::cerr << "recv: " << std::strerror(errno) << std::endl;
		close(clientFd);
		return (false);
	}
	// Disconnect
	else if (res == 0)
	{
	close(clientFd);
	this->_pollFds.erase(this->_pollFds.begin() + index);
		this->_clients.erase(clientFd);
		std::cout << "Client " << clientFd << " disconnected" << std::endl;
		return (true);
	}
	// No problem
	client->appendReceivedData(buff, res);
	if (client->hasCompleteHeaders())
	{
		// We received all of the HTTP header
		// that not mean that we received all, there's maybe a body so if we read enougth bytes
		// (number of bytes are indicated by Content-Length)
		client->stashHeaders();
	}
	else if (client->getContentLength() == client->getNbBodyByte())
	{
		client->stashBody();
		this->_pollFds[index].events = POLLOUT | POLLIN;
	}
}

bool	Server::clientPOLLOUT(Client *client, int clientFd, int index)
{
	if (!client->hasSomethingToSend())
	{
		this->_pollFds[index].events = POLLIN;
		return (true);
	}

	const std::string &response = client->getSendBuffer();;
	size_t	byte_send = send(clientFd, response.c_str(), response.length(), 0);
	client->removeReponseSend(byte_send);

	return (true);
}