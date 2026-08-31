/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 15:50:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/31 18:22:58 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <WebservInclude.h>

class Client
{
private:
	int _fd;
	std::string _recvBuffer;
	std::string _sendBuffer;
public:
	Client();
	Client(int fd);
	~Client();

	void	retrieveData(char	*buff, int len);

	// === GETTER === //
	int	getFd() const;
};

#endif