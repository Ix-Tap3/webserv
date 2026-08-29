/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 20:22:45 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 22:02:54 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "../../includes/json/JsonParser.hpp"

JsonParser::JsonParser	( std::vector<Token> &tokenList ): _tokenList(tokenList), _pos(0) { }

bool	JsonParser::isEnd( void ) const { return this->_pos >= this->_tokenList.size(); }
bool	JsonParser::check( TokenType type ) const { return this->peek().type == type; }

Token	JsonParser::peek( void ) const
{
	if (this->isEnd())
		return Token();
	return this->_tokenList[this->_pos];
}

static std::string	strTokenType( TokenType type )
{
	switch (type)
	{
		case TOKEN_LBRACE:
			return std::string("LBRACE");
		case TOKEN_RBRACE:
			return std::string("RBRACE");
		case TOKEN_LBRACKET:
			return std::string("LBRACKET");
		case TOKEN_RBRACKET:
			return std::string("RBRACKET");
		case TOKEN_COLON:
			return std::string("COLON");
		case TOKEN_COMMA:
			return std::string("COMMA");
		case TOKEN_BOOL:
			return std::string("BOOL");
		case TOKEN_STRING:
			return std::string("STRING");
		case TOKEN_NUMBER:
			return std::string("NUMBER");
		case TOKEN_END:
			return std::string("END");
		case TOKEN_NULL:
			return std::string("NULL");
	}
	return std::string();
}

Token	JsonParser::expect( TokenType type )
{
	Token	current = this->peek();

	if (current.type != type)
	{
		std::string	msg("Unexpected " + strTokenType(current.type) + " token, value: <"
				  + current.value + "> at index: ");
		std::stringstream	ss;

		ss << this->_pos << ", expected " << strTokenType(type) << " token";
		msg.append(ss.str());
		throw JsonParseException(msg);
	}
	return this->advance();
}

Token	JsonParser::advance( void )
{
	if (this->isEnd())
		return Token();

	std::size_t	tmp = this->_pos;

	this->_pos++;

	return this->_tokenList[tmp];
}

JsonValue	JsonParser::parse( void )
{
	Token		token;
	JsonValue	value;

	token = expect(TOKEN_COLON);

	return value;
}

// --- Exceptions
JsonParser::JsonParseException::~JsonParseException	( void ) throw() { }
JsonParser::JsonParseException::JsonParseException	( std::string msg )
{
	this->_msg = "JsonParser Error: " + msg;
}
const char	*JsonParser::JsonParseException::what( void ) const throw()
{
	return this->_msg.c_str();
}

