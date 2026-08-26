/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:05:19 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/26 17:26:33 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <vector>
# include "JsonToken.hpp"

class JsonLexer
{
private:
	std::string	_input;
	std::size_t	_pos;

	bool		isEnd( void ) const;
	char		peek( void ) const;
	char		advance( void );
	void		skipWhiteSpaces( void );
	Token		lexString( void );
	Token		lexNumber( void );
	Token		lexSymbol( void );
	Token		lexKeyword( void );
	std::string	fileToString( std::string &filename );

public:
	JsonLexer	( std::string &fileName );

	std::vector<Token>	tokenize();
};
