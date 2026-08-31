/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 22:07:40 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client()
{
}

Client::Client(int fd)
{
	this->_fd = fd;
}

Client::~Client()
{
}

int	Client::getFd() const
{
	return (this->_fd);
}


// === RECEIVE DATA === //
void	Client::appendReceivedData(char	*buff, int len)
{
	this->_recvBuffer.append(buff, len);
	std::cout.write(buff, len);
	std::cout << "Client " << this->_fd << "received buffer: " << this->_recvBuffer << std::endl;
}

bool	Client::hasCompleteHeaders() const
{
	return (this->_recvBuffer.find("\r\n\r\n") != std::string::npos);
}

// === SEND DATA === //
void	Client::appendSendData(std::string data)
{
	this->_sendBuffer += data;
}
