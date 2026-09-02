/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/01 02:21:41 by anfouger         ###   ########.fr       */
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

// === GETTER === //
int	Client::getFd() const
{
	return (this->_fd);
}

int	Client::getNbBodyByte() const
{
	return (this->_nbBodyByte);
}

int	Client::getContentLength() const
{
	return (this->_httpRequest.contentLength);
}

// === RECEIVE DATA === //
void	Client::appendReceivedData(char	*buff, int len)
{
	this->_recvBuffer.append(buff, len);
	// std::cout.write(buff, len);
	// std::cout << "Client " << this->_fd << "received buffer: " << this->_recvBuffer << std::endl;
}

bool	Client::hasCompleteHeaders() const
{
	return (this->_recvBuffer.find("\r\n\r\n") != std::string::npos);
}

// === SEND DATA === //
std::string	Client::sendResponse()
{
	return (this->_sendBuffer.data());
}

bool	Client::hasSomethingToSend() const
{
	return (!this->_sendBuffer.empty());
}

void	Client::appendSendData(std::string data)
{
	this->_sendBuffer += data;
}

void	Client::stashHeaders()
{
	this->_httpRequest.header = this->_recvBuffer;
	this->_recvBuffer.clear();
}

void	Client::stashBody()
{
	this->_httpRequest.body = this->_recvBuffer;
	this->_recvBuffer.clear();
}

void	Client::removeReponseSend(size_t byte_send)
{
	this->_sendBuffer.erase(0, byte_send);
}
