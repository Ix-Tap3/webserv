/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/24 21:37:59 by anfouger          #+#    #+#             */
/*   Updated: 2026/08/29 16:24:35 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <cerrno>

#include <unistd.h>
#include <string.h>
#include "./Class/Server.hpp"

// std::map<std::string, std::string>	&createAddressPort(void)
// {
// 	std::map<std::string, std::string>  address_port;

// 	address_port.insert(std::make_pair("127.0.0.1", "8080"));
// 	address_port.insert(std::make_pair("192.168.0.1", "8080"));
// 	address_port.insert(std::make_pair("192.168.0.2", "8080"));

// 	return (address_port);
// }

int main(void)
{
    Server server;

    if (!server.setup())
        return 1;

    server.run();
	while (true)
		;
	return 0;
}
