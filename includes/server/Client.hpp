/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/23 10:52:45 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "../../includes/WebservInclude.h"
# include "../../includes/Utils/Utils.hpp"

# include "../../includes/Http/HttpParser.hpp"
# include "../../includes/Http/HttpRequestHandler.hpp"
# include "../../includes/Http/HttpResponse.hpp"
# include <ctime>

struct State
{
	bool	header;
	bool	body;
	bool	connection;
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

	HttpParser	_parser;
	HttpRequestHandler _httpRequestHandler;
	HttpResponse		_httpResponseMaker;

	int			extractContentLength();
	bool		extractConnection();
	bool		isClose(std::string value);
	bool		isKeepAlive(std::string value);
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
