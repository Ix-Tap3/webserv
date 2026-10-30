/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:03 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/25 15:34:55 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../../includes/server/Client.hpp"


void	Client::printHeader()
{
	std::cout << std::endl << "// === REQUEST LINE === //" << std::endl;
	std::cout << "Method: " << this->_httpRequest.requestLine._method << std::endl;
	std::cout << "Target: " << this->_httpRequest.requestLine._target << std::endl;
	std::cout << "Version: " << this->_httpRequest.requestLine._version << std::endl;

	std::cout << "// === HEADERS FIELDS === //" << std::endl;
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest.header._headersFields.begin(); 
		it != this->_httpRequest.header._headersFields.end(); 
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
	this->_state.chunked = false;
	this->_maxBodySize = 0;
	this->_nbBodyByte = 0;
	this->_lastActivity = std::time(NULL);
}

Client::Client(int fd, const ServerConfig *config): _httpRequestHandler(config)
{
	this->_fd = fd;
	this->_contentLength = -1;
	this->_state.header = true;
	this->_state.body = false;
	this->_state.connection = true;
	this->_state.chunked = false;
	this->_maxBodySize = 0;
	this->_nbBodyByte = 0;
	this->_lastActivity = std::time(NULL);
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

time_t	Client::getLastActivityTime() const
{
	return this->_lastActivity;
}

//======================//
// ====== SETTER ====== //
//======================//
void	Client::setActivityTime() { this->_lastActivity = std::time(NULL); }

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
		this->_httpRequest.header._headersFields.begin(); 
		it != this->_httpRequest.header._headersFields.end(); 
		++it)
	{
		std::string name = Utils::strToMin((*it).first);
		if (name == "content-length")
			return (parseContentLength((*it).second));
	}
	return (0);
}

bool		Client::extractConnection()
{
	bool state = false;
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest.header._headersFields.begin(); 
		it != this->_httpRequest.header._headersFields.end(); 
		++it)
	{
		std::string name = Utils::strToMin((*it).first);
		std::string value = Utils::strToMin((*it).second);
		if (name == "connection")
		{
			// size_t pos = value.find("close");
			if (isClose(value))
				return (false);
			// pos = value.find("keep-alive");
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

bool	Client::stashHeaders()
{
	size_t endOfHeader = this->_recvBuffer.find("\r\n\r\n");
	this->_strHeader = this->_recvBuffer.substr(0, endOfHeader + 4);
	this->_recvBuffer.erase(0, endOfHeader + 4);

	this->_nbBodyByte += this->_recvBuffer.length();
	
	try
	{
		this->_parser = HttpParser();
		this->_httpRequest = this->_parser.ParseHttpRequest(this->_strHeader);
		printHeader(); // for test purpose
	}
	catch(const HttpException& e)
	{
		this->prepareErrorResponse(e.getStatusCode());
		return (false);
	}
	
	this->_contentLength = extractContentLength();
	this->_maxBodySize = this->_httpRequestHandler.getMaxContentLength(this->_httpRequest.requestLine._target);

	if (this->_contentLength > this->_maxBodySize || this->_contentLength == CONTENT_LENGTH_TOO_LARGE)
	{
		prepareErrorResponse(413);
		return (false);
	}
	this->_state.connection = extractConnection();
	if (isChunked())
	{
		this->_state.header = false;
		this->_state.chunked = true;
		this->_chunkedBody.clear();
		return (true);
	}
	if (this->_contentLength > 0)
	{
		this->_state.header = false;
		this->_state.body = true;
	}
	return (true);
}

bool	Client::isChunked()
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest.header._headersFields.begin();
		it != this->_httpRequest.header._headersFields.end();
		++it)
	{
		if (Utils::strToMin(it->first) == "transfer-encoding")
			return (true);
	}
	return (false);
}

bool	Client::isReadingChunks() const
{
	return (this->_state.chunked);
}

ChunkStatus	Client::decodeChunks(int &errorCode)
{
	while (true)
	{
		size_t	lineEnd = this->_recvBuffer.find("\r\n");
		if (lineEnd == std::string::npos)
		{
			if (this->_recvBuffer.size() > MAX_CHUNK_LINE)
			{
				errorCode = 400;
				return (CHUNK_ERROR);
			}
			return (CHUNK_NEED_MORE);
		}

		size_t	chunkSize;
		if (!parseChunkSize(this->_recvBuffer.substr(0, lineEnd), chunkSize))
		{
			errorCode = 400;
			return (CHUNK_ERROR);
		}

		if (chunkSize == 0)
		{
			size_t	end = this->_recvBuffer.find("\r\n\r\n", lineEnd);
			if (end == std::string::npos)
				return (CHUNK_NEED_MORE);
			this->_recvBuffer.erase(0, end + 4);
			return (CHUNK_DONE);
		}

		if (chunkSize > static_cast<size_t>(this->_maxBodySize) - this->_chunkedBody.size())
		{
			errorCode = 413;
			return (CHUNK_ERROR);
		}

		size_t	dataStart = lineEnd + 2;
		if (this->_recvBuffer.size() < dataStart + chunkSize + 2)
			return (CHUNK_NEED_MORE);
		if (this->_recvBuffer.compare(dataStart + chunkSize, 2, "\r\n") != 0)
		{
			errorCode = 400;
			return (CHUNK_ERROR);
		}

		this->_chunkedBody.append(this->_recvBuffer, dataStart, chunkSize);
		this->_recvBuffer.erase(0, dataStart + chunkSize + 2);
	}
}

bool	Client::parseChunkSize(const std::string &line, size_t &size) const
{
	std::string	hex = line.substr(0, line.find(';'));

	Utils::DeleteUselessSpace(hex);
	if (hex.empty() || hex.size() > 8)
		return (false);

	size = 0;
	for (size_t i = 0; i < hex.size(); i++)
	{
		char	c = hex[i];
		int		digit;

		if (c >= '0' && c <= '9')
			digit = c - '0';
		else if (c >= 'a' && c <= 'f')
			digit = c - 'a' + 10;
		else if (c >= 'A' && c <= 'F')
			digit = c - 'A' + 10;
		else
			return (false);
		size = size * 16 + digit;
	}
	return (true);
}

bool	Client::processChunkedBody()
{
	int			errorCode = 0;
	ChunkStatus	status = decodeChunks(errorCode);

	if (status == CHUNK_NEED_MORE)
		return (false);
	if (status == CHUNK_ERROR)
	{
		prepareErrorResponse(errorCode);
		return (true);
	}
	this->_httpRequest._body.swap(this->_chunkedBody);
	this->_chunkedBody.clear();
	finishRequest();
	return (true);
}

void	Client::stashBody()
{
	this->_httpRequest._body = this->_recvBuffer.substr(0, this->_contentLength);
	this->_recvBuffer.erase(0, this->_contentLength);
	finishRequest();
}

void	Client::finishRequest()
{
	std::cout << "[debug] body: " << this->_httpRequest._body.size() << " bytes" << std::endl;

	this->_state.body = false;
	this->_state.chunked = false;
	this->_state.header = true;
	this->_nbBodyByte = 0;
	this->_contentLength = -1;

	this->_sendBuffer = this->_httpResponseMaker.CreateHttpResponse(this->_httpRequestHandler.handleRequest(this->_httpRequest));
}

//this->_httpRequestHandler.handleRequest(this->_httpRequest)
void	Client::removeReponseSend(size_t byte_send)
{
	this->_sendBuffer.erase(0, byte_send);
}

bool	Client::hasPendingRequest( void ) const
{
	if (!this->_recvBuffer.empty() || this->_state.body || this->_state.chunked)
		return true;
	return false;
}

void	Client::prepareErrorResponse( int code )
{
	ResponseData	err = this->_httpRequestHandler.handleError(code);

	this->_sendBuffer = this->_httpResponseMaker.CreateHttpResponse(err);

	this->_state.connection = false;
	this->_state.body = false;
	this->_state.chunked = false;
	this->_chunkedBody.clear();
	this->_state.header = true;
	this->_recvBuffer.clear();
	this->_contentLength = -1;
	this->_nbBodyByte = 0;
}

int	Client::parseContentLength(const std::string &value) const
{
	std::stringstream	ss(value);
	long				nb;

	ss >> nb;
	if (ss.fail() || nb > std::numeric_limits<int>::max())
		return (CONTENT_LENGTH_TOO_LARGE);

	return (static_cast<int>(nb));
}
