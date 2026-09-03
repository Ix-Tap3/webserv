/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 19:13:14 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

Client::Client()
{
	this->_contentLength = -1;
}

Client::Client(int fd)
{
	this->_fd = fd;
	this->_contentLength = -1;
}

Client::~Client()
{
}

//======================//
// ====== GETTER ====== //
//======================//
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
	return (this->_contentLength);
}

const std::string&	Client::getSendBuffer() const
{
	return (this->_sendBuffer);
}

//======================//
// === RECEIVE DATA === //
//======================//
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

//======================//
// ===== SEND DATA ==== //
//======================//
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
	this->_strHeader = this->_recvBuffer;
	this->_recvBuffer.clear();
	try
	{
		this->_httpRequest._Header = this->_parser.ParseHeader(this->_strHeader);
	}
	catch(const HttpException& e)
	{
		std::cerr << e.getStatusCode() << " ";
		std::cerr << e.what() << std::endl;
	}
}

void	Client::stashBody()
{
	this->_strBody = this->_recvBuffer;
	this->_recvBuffer.clear();
}

void	Client::removeReponseSend(size_t byte_send)
{
	this->_sendBuffer.erase(0, byte_send);
}
