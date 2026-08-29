/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 11:36:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 11:34:10 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "includes/json/JsonLexer.hpp"

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

	return 0;
}
