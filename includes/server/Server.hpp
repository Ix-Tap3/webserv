/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 20:58:53 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/23 09:49:46 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <WebservInclude.h>
# include <Client.hpp>
# include <configs.h>

class Server
{
private:
	const ServerConfig			&_config;
	int							_socketFd;
	std::vector<int>			_socketFds;
	std::vector<struct pollfd>	_pollFds;
	std::map<int, int>			_cgisFds; // int: pipeFd, int: clientFd
	std::vector<int>			_fdsToClose;
	std::map<int, Client>		_clients;

	// === SETUP PART === //
	bool			setupListener(int port);
	struct pollfd	setupServerPoll();
	bool			setupListen(struct addrinfo *res);
	bool			setupBind(struct addrinfo *res);
	bool			setupSocket(struct addrinfo *res);

	// === RUN PART === //
	// Server Socket //
	bool	isServerSocket(int fd) const;
	bool	watchServerSocket(int index);
	bool	acceptNewClient(int serverFd);
	// Clients Socket //
	bool	watchClientsSocket(int index);
	bool	clientPOLLIN(Client *client, int clientFd, int index);
	bool	clientPOLLOUT(Client *client, int clientFd, int index);
	// Utils //
	bool	isInFdCloseList( int fd ) const;
	bool	isClientFd( int fd ) const;
	void	clearFds( void );
	void	addFdToCloseList( int fd );
	void	rebuildPollFds( void );
	void	handleInactiveClient( void );

public:
	Server(const ServerConfig &config);
	~Server();

	bool	setup();
	bool	run();
};

#endif
