/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/27 16:28:49 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

void	Client::printHeader()
{
	std::cout << "// === REQUEST LINE === //";
	std::cout << "Method: " << this->_httpRequest._requestLine.method;
	std::cout << "Target: " << this->_httpRequest._requestLine.target;
	std::cout << "Version: " << this->_httpRequest._requestLine.version;

	std::cout << "// === HEADERS FIELDS === //";
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::cout << "\"" << it->first << "\": \"" << it->second << "\"";
	}
}

Client::Client()
{
	this->_contentLength = -1;
	this->_state.header = true;
	this->_state.body = false;
	this->_state.connection = true;
	this->_nbBodyByte = 0;
}

Client::Client(int fd)
{
	this->_fd = fd;
	this->_contentLength = -1;
	this->_state.header = true;
	this->_state.body = false;
	this->_state.connection = true;
	this->_nbBodyByte = 0;
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

bool	Client::getConnectionState() const
{
	return (this->_state.connection);
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
	if (this->_state.body)
		this->_nbBodyByte += len;
	
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
int		Client::extractContentLength()
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::string name = Utils::strToMin((*it).first);
		if (name == "content-length")
			return (Utils::stringToInt((*it).second));
	}
	return (0);
}

bool		Client::extractConnection()
{
	bool state = false;
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::string name = Utils::strToMin((*it).first);
		std::string value = Utils::strToMin((*it).second);
		if (name == "connection")
		{
			size_t pos = value.find("close");
			if (isClose(value))
				return (false);
			size_t pos = value.find("keep-alive");
			if (isKeepAlive(value))
				state = true;
		}
	}
	return (state);
}

bool	isClose(std::string value)
{
	std::vector<std::string> token = Utils::strSplit(value, ',');
	for (std::vector<std::string>::iterator it = token.begin();
		it != token.end(); it++)
	{
		Utils::DeleteUselessSpace((*it));
		if ((*it) == "close")
			return (true);
	}
	return (false);
}

bool	isKeepAlive(std::string value)
{
	std::vector<std::string> token = Utils::strSplit(value, ',');
	for (std::vector<std::string>::iterator it = token.begin();
		it != token.end(); it++)
	{
		Utils::DeleteUselessSpace((*it));
		if ((*it) == "keep-alive")
			return (true);
	}
	return (false);
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
	size_t endOfHeader = this->_recvBuffer.find("\r\n\r\n");
	
	this->_strHeader = this->_recvBuffer.substr(0, endOfHeader + 4);

	this->_recvBuffer.erase(0, endOfHeader + 4);

	this->_nbBodyByte += this->_recvBuffer.length();
	
	try
	{
		this->_httpRequest._header = this->_parser.ParseHeader(this->_strHeader);
		printHeader(); // for test purpose
	}
	catch(const HttpException& e)
	{
		std::cerr << e.getStatusCode() << " ";
		std::cerr << e.what() << std::endl;
	}
	this->_contentLength = extractContentLength();
	if (this->_contentLength > 0)
	{
		this->_state.header = false;
		this->_state.body = true;
	}
	this->_state.connection = extractConnection();
}

void	Client::stashBody()
{
	this->_strBody = this->_recvBuffer;
	this->_recvBuffer.clear();
	this->_state.body = false;
	this->_state.header = true;
	this->_nbBodyByte = 0;
}

void	Client::removeReponseSend(size_t byte_send)
{
	this->_sendBuffer.erase(0, byte_send);
}
