/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tseche <tseche@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:36:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/14 12:22:01 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
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
		displayTokenList(tokenList);
		std::cout << strTokenType(tokenList[9].type) << " " << tokenList[9].pos.line << " " << tokenList[9].pos.col << std::endl;
		// std::cout << lexer.getSrc();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	JsonValue	root;

	try
	{
		JsonParser	parser(tokenList);

		root = parser.parse();
		// displayJsonTree(root);
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	ServerConfig	data;

	try
	{
		ConfigLoader	loader(root);

		data = loader.load();
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }
	
	// Server server;
	//
	// if (!server.setup())
	// 	return 1;
	//
	// server.run();
	// while (true)
	// 	;
	return 0;
}
