/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 20:58:53 by anfouger          #+#    #+#             */
/*   Updated: 2026/11/02 19:47:38 by pcaplat          ###   ########.fr       */
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

	const std::vector<ServerConfig>	&_configs;
	std::map<int, const ServerConfig *>	_configMap; // int: socketFd, ServerConfig: Associated server configuration
	int							_socketFd;
	std::vector<int>			_socketFds;
	std::vector<struct pollfd>	_pollFds;
	std::map<int, int>			_cgisFds; // int: pipeFd, int: clientFd
	std::vector<int>			_fdsToClose;
	std::map<int, Client>		_clients;

	// === SETUP PART === //
	bool			setupListener(int port, const ServerConfig &config);
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
	int		getClientPosByFd(int fd) const;

public:
	Server(const std::vector<ServerConfig> &configs);
	~Server();

	bool	setup();
	bool	run();
};

#endif
