/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:36:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/30 13:09:48 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "includes/json/JsonLexer.hpp"
#include "includes/json/JsonParser.hpp"
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
		std::string	filename(av[1]);
		JsonLexer	lexer(filename);

		tokenList = lexer.tokenize();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	try
	{
		JsonParser	parser(tokenList);
		JsonValue	root;

		root = parser.parse();
		displayJsonTree(root);
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

    Server server;

    if (!server.setup())
        return 1;

	server.run();
	while (true)
		;
	return 0;
}
