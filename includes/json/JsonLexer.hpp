/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:05:19 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/09 11:34:42 by pcaplat          ###   ########.fr       */
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
	std::size_t	_currentLine;
	std::size_t	_currentCol;

	bool		isEnd( void ) const;
	char		peek( void ) const;
	char		advance( void );
	void		skipWhiteSpace( void );
	void		setTokenPos( TokenPos &pos ) const;
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

	class JsonSyntaxException:	public std::exception
	{
	private:
		std::string	_msg;

	public:
		JsonSyntaxException		( std::string msg );
		~JsonSyntaxException	( void );

		const char	*what( void ) const throw();
	};

	class JsonUnexpectedTokenException:	public std::exception
	{
	private:
		std::string	_msg;
	
	public:
		JsonUnexpectedTokenException	( Token token );
		JsonUnexpectedTokenException	( char value, std::size_t line, std::size_t col );
		~JsonUnexpectedTokenException	( void );

		const char	*what( void ) const throw();
	};

public:
	JsonLexer	( std::string &fileName );

	std::string	getSrc( void ) const;

	std::vector<Token>	tokenize();
};

// --- Debug (REMOVE BEFORE PUSH)

void	displayTokenList( std::vector<Token> &tokenList );
std::ostream	&operator<<	( std::ostream &out, const JsonLexer &lex );
