/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:33:15 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <WebservInclude.h>
# include <Utils.hpp>

# include <HttpParser.hpp>
# include <HttpRequestHandler.hpp>
# include <HttpResponse.hpp>

struct State
{
	bool	header;
	bool	body;
	bool	connection;
};

class Client
{
private:
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
	Client(int fd);
	~Client();

	void	printHeader();

	// === RECEIVE DATA === //
	void	appendReceivedData(char	*buff, int len);
	bool	hasCompleteHeaders() const;
	void	stashHeaders();
	void	stashBody();

	// === SEND DATA === //
	void		appendSendData(std::string data);
	bool		hasSomethingToSend() const;
	const std::string&	getSendBuffer() const;
	void		removeReponseSend(size_t byte_send);

	// === GETTER === //
	int		getFd() const;
	int		getNbBodyByte() const;
	int		getContentLength() const;
	bool	getConnectionState() const;
};

#endif
