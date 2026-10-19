/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:36:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/19 13:19:56 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include "includes/json/JsonLexer.hpp"
#include "includes/json/JsonParser.hpp"
#include "includes/json/JsonToken.hpp"
#include "includes/server/configs.h"
#include "includes/server/ConfigLoader.hpp"
#include "includes/server/Server.hpp"

int	main( int ac, char **av )
{
	if (ac != 2)
	{
		std::cerr << "Error: please use: ./webserv <configuration_file>." << std::endl;
		return 1;
	}

	std::vector<Token>	tokenList;

	try
	{
		JsonLexer	lexer(av[1]);

		tokenList = lexer.tokenize();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	JsonValue	root;

	try
	{
		JsonParser	parser(tokenList);

		root = parser.parse();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	ServerConfig	data;

	try
	{
		ConfigLoader	loader(root);

		data = loader.load();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	std::signal(SIGPIPE, SIG_IGN);

	Server server(data);

	if (!server.setup())
		return (1);
	if (!server.run())
		return (1);
	return (0);
}
