/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/25 15:33:42 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP
# define CONTENT_LENGTH_TOO_LARGE	-2
# define MAX_CHUNK_LINE				4096

# include "../../includes/WebservInclude.h"
# include "../../includes/Utils/Utils.hpp"

# include "../../includes/Http/HttpParser.hpp"
# include "../../includes/Http/HttpRequestHandler.hpp"
# include "../../includes/Http/HttpResponse.hpp"
# include <ctime>
# include <limits>

struct State
{
	bool	header;
	bool	body;
	bool	connection;
	bool	chunked;
};

enum ChunkStatus
{
	CHUNK_NEED_MORE,
	CHUNK_DONE,
	CHUNK_ERROR
};

class Client
{
private:

	time_t		_lastActivity;
	int _fd;
	std::string _recvBuffer;

	std::string _strHeader;

	HttpRequest	_httpRequest;
	std::string _sendBuffer;

	State		_state;
	int			_nbBodyByte;
	int			_contentLength;
	int			_maxBodySize;
	std::string	_chunkedBody;

	HttpParser	_parser;
	HttpRequestHandler _httpRequestHandler;
	HttpResponse		_httpResponseMaker;

	int			extractContentLength();
	bool		extractConnection();
	bool		isClose(std::string value);
	bool		isKeepAlive(std::string value);
	int			parseContentLength(const std::string &value) const;
	bool		isChunked();
	ChunkStatus	decodeChunks(int &errorCode);
	bool		parseChunkSize(const std::string &line, size_t &size) const;
	void		finishRequest();
public:

	Client();
	Client(int fd, const ServerConfig *config);
	~Client();

	void	printHeader();

	// === RECEIVE DATA === //
	void	appendReceivedData(char	*buff, int len);
	bool	hasCompleteHeaders() const;
	bool	stashHeaders();
	void	stashBody();
	bool	isReadingChunks() const;
	bool	processChunkedBody();

	// === SEND DATA === //
	void				appendSendData(std::string data);
	bool				hasSomethingToSend() const;
	const std::string&	getSendBuffer() const;
	void				removeReponseSend(size_t byte_send);

	// === GETTER === //
	int		getFd() const;
	int		getNbBodyByte() const;
	int		getContentLength() const;
	bool	getConnectionState() const;
	time_t	getLastActivityTime() const;
	
	// === SETTER === //
	void	setActivityTime();

	// === CLIENT TIMEOUT === //
	bool	hasPendingRequest() const;

	// === ERRORS === //
	void	prepareErrorResponse( int code );
};

#endif
