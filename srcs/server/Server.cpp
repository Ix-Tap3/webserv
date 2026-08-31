/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 21:06:27 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 18:24:16 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"


Server::Server()
{
}

Server::~Server()
{
}

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

	this->_socketFd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (this->_socketFd == -1)
	{
		std::cerr << "socket: " << std::strerror(errno) << std::endl;
		freeaddrinfo(res);
		return (false);
	}
	std::cout << "Socket successful" << std::endl;

	int res_bind = bind(this->_socketFd, res->ai_addr, res->ai_addrlen);
	if (res_bind == -1)
	{
		std::cerr << "bind: " << std::strerror(errno) << std::endl;
		close(this->_socketFd);
		freeaddrinfo(res);
		return (false);
	}
	std::cout << "Bind successful" << std::endl;

	int res_listen = listen(this->_socketFd, 10);
	if (res_listen == -1)
	{
		std::cerr << "listen: " << std::strerror(errno) << std::endl;
		freeaddrinfo(res);
		close(this->_socketFd);
		return (false);
	}
	std::cout << "Listen successful" << std::endl;

	struct pollfd server_poll;
	server_poll.fd = this->_socketFd;
	server_poll.events = POLLIN;
	server_poll.revents = 0;
	this->_pollFds.push_back(server_poll);

	freeaddrinfo(res);
	return (true);
}

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

bool	Server::watchClientsSocket(int index)
{
	if (this->_pollFds[index].revents & POLLIN)
	{
		char	buff[100];
		int		client_fd = this->_pollFds[index].fd;

		int	res = recv(client_fd, buff, sizeof(buff), 0);
		// Error
		if (res == -1)
		{
			std::cerr << "recv: " << std::strerror(errno) << std::endl;
			close(client_fd);
			return (false);
		}
		// Disconnect
		else if (res == 0)
		{
			close(client_fd);
			this->_pollFds.erase(this->_pollFds.begin() + index);
			this->_clients.erase(client_fd);
			std::cout << "Client " << client_fd << " disconnected" << std::endl;
			return (true);
		}
		// No problem
		this->_clients[client_fd].retrieveData(buff, res);
	}
	return (true);
}
