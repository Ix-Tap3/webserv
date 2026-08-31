/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 18:18:21 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

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

void	Client::retrieveData(char	*buff, int len)
{
	this->_recvBuffer += buff;
	std::cout.write(buff, len);
	std::cout << "Client " << this->_fd << " Talk!" << std::endl;
}
