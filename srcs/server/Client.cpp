/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 20:54:13 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/server/Client.hpp"

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

void	Client::appendReceivedData(char	*buff, int len)
{
	this->_recvBuffer.append(buff, len);
	std::cout.write(buff, len);
	std::cout << "Client " << this->_fd << " Talk!" << std::endl;
}

void	Client::appendSendData(std::string data)
{
	this->_sendBuffer += data;
}
