/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:36:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 12:15:40 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "includes/json/JsonLexer.hpp"
#include "includes/json/JsonValue.hpp"

int	main( int ac, char **av )
{
	if (ac != 2)
	{
		std::cerr << "Error: please use: ./webserv <configuration_file>." << std::endl;
		return 1;
	}

	try
	{
		std::string	filename(av[1]);
		JsonLexer	lexer(filename);

		std::cout << "lexer build !" << std::endl;
		std::cout << lexer << std::endl;

		std::vector<Token>	token_list = lexer.tokenize();
		displayTokenList(token_list);
	} catch ( std::exception &e ) { std::cerr << e.what() << std::endl; return 1; }

	JsonValue	num( JSON_NUMBER );

	return 0;
}
