/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:05:19 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/26 17:32:17 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <vector>
# include <exception>
# include "JsonToken.hpp"

class JsonLexer
{
private:
	std::string	_input;
	std::size_t	_pos;

	bool		isEnd( void ) const;
	char		peek( void ) const;
	char		advance( void );
	void		skipWhiteSpace( void );
	Token		lexString( void );
	Token		lexNumber( void );
	Token		lexSymbol( void );
	Token		lexKeyword( void );

	class JsonLexerException:	public std::exception
	{
	private:
		std::string	_msg;

	public:
		JsonLexerException	( std::string msg );
		~JsonLexerException	( void );

		const char	*what( void ) const throw();
	};

public:
	JsonLexer	( std::string &fileName );

	std::string	getSrc( void ) const;

	std::vector<Token>	tokenize();
};

std::ostream	&operator<<	( std::ostream &out, const JsonLexer &lex );
