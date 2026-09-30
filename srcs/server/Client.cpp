/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/30 22:06:21 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Client.hpp>

void	Client::printHeader()
{
	std::cout << std::endl << "// === REQUEST LINE === //" << std::endl;
	std::cout << "Method: " << this->_httpRequest._requestLine.method << std::endl;
	std::cout << "Target: " << this->_httpRequest._requestLine.target << std::endl;
	std::cout << "Version: " << this->_httpRequest._requestLine.version << std::endl;

	std::cout << "// === HEADERS FIELDS === //" << std::endl;
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::cout << "\"" << it->first << "\": \"" << it->second << "\"" << std::endl;
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
	if (this->_state.body)
	{
		this->_nbBodyByte += len;
		this->_recvBuffer.append(buff, len);
	}
	else
		this->_recvBuffer.append(buff, len);
}

bool	Client::hasCompleteHeaders() const
{    
    size_t pos = this->_recvBuffer.find("\r\n\r\n"); // test purpose

    if (pos != std::string::npos) // test purpose
    {
        std::cout << "The header is complete at position " // test purpose
                  << pos << std::endl; // test purpose
        return (true); // test purpose
    }

    return (false); // test purpose
	// return (this->_recvBuffer.find("\r\n\r\n") != std::string::npos); // real code
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
			pos = value.find("keep-alive");
			if (isKeepAlive(value))
				state = true;
		}
	}
	return (state);
}

bool	Client::isClose(std::string value)
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

bool	Client::isKeepAlive(std::string value)
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
		this->_parser = HttpParser();
		this->_httpRequest = this->_parser.ParseHeader(this->_strHeader);
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
	std::string body = this->_recvBuffer.substr(0, this->_contentLength);
	this->_recvBuffer.erase(0, this->_contentLength);
	this->_strBody = body;

	this->_state.body = false;
	this->_state.header = true;
	this->_nbBodyByte = 0;

	this->_sendBuffer = "yes";
}

void	Client::removeReponseSend(size_t byte_send)
{
	this->_sendBuffer.erase(0, byte_send);
}
