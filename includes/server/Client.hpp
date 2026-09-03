/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 17:01:05 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <WebservInclude.h>
# include <HttpParser.hpp>

class Client
{
private:
	int _fd;
	std::string _recvBuffer;
	std::string _strHeader;
	std::string _strBody;
	std::string _sendBuffer;
	int			_nbBodyByte;
	int			_contentLength;
	HttpRequest	_httpRequest;

	HttpParser	_parser;
public:

	Client();
	Client(int fd);
	~Client();

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
	int	getFd() const;
	int	getNbBodyByte() const;
	int	getContentLength() const;
};

#endif
