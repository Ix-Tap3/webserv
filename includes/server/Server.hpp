/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/27 20:58:53 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 17:50:27 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <WebservInclude.h>
# include "Client.hpp"

class Server
{
private:
	int	_socketFd;
	std::vector<struct pollfd> _pollFds;
	std::map<int, Client>		_clients;

	// === SETUP PART === //

	// === RUN PART === //
	// Server Socket //
	bool	watchServerSocket(int index);
	bool	acceptNewClient();
	// Clients Socket //
	bool	watchClientsSocket(int index);

public:
	Server();
	~Server();

	bool	setup();
	bool	run();
};

#endif