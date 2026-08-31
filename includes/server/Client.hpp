/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/01 01:01:07 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <WebservInclude.h>

struct HttpRequest
{
	std::string header;
	std::string body;
	int			contentLength;
};

class Client
{
private:
	int _fd;
	std::string _recvBuffer;
	std::string _sendBuffer;
	int			_nbBodyByte;
	HttpRequest	_httpRequest;
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
	void	appendSendData(std::string data);
	bool	hasSomethingToSend() const;
	char 	*sendResponse();
	void	removeReponseSend(size_t byte_send);

	// === GETTER === //
	int	getFd() const;
	int	getNbBodyByte() const;
	int	getContentLength() const;
};

#endif
