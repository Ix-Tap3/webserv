/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:27:42 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/30 13:22:29 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cctype>
#include "../../includes/json/JsonLexer.hpp"

static bool	checkFileExtension( std::string &filename, std::string extension)
{
	std::size_t	found = filename.find_last_of(".");
	if (found == std::string::npos)
		return false;

	std::string	tmp = filename.substr(found + 1);
	if (tmp != extension)
		return false;

	return true;
}

// Constructor
JsonLexer::JsonLexer	( std::string &filename ): _pos(0)
{
	if (filename.empty())
		throw JsonLexerException("Empty filename provided");
	if (!checkFileExtension(filename, "json"))
		throw JsonLexerException("Invalid file extension, please use only json files");

	std::ifstream	file(filename);

	if (!file.is_open())
		throw JsonLexerException("Cannot open file named: " + filename);
	if (file.peek() == std::ifstream::traits_type::eof())
		throw JsonLexerException("Invalid empty configuration file provided");

	std::string	line;

	while (std::getline(file, line))
		this->_input.append(line);
}

// Member functions
bool	JsonLexer::isEnd( void ) const { return this->_pos >= this->_input.length(); }

char	JsonLexer::peek( void ) const
{
	if (this->isEnd())
		return '\0';
	return this->_input[this->_pos];
}

char	JsonLexer::advance( void )
{
	if (this->isEnd())
		return '\0';
	std::size_t	idx = this->_pos;
	this->_pos++;
	return this->_input[idx];
}

void	JsonLexer::skipWhiteSpace( void )
{
	if (this->isEnd())
		return;
	while (!this->isEnd() && std::isspace(this->_input[this->_pos]))
		this->_pos++;
}

std::vector<Token>	JsonLexer::tokenize()
{
	std::vector<Token>	tokenList;

	while (!this->isEnd())
	{
		this->skipWhiteSpace();
		char	c = this->peek();
		Token	token;

		switch (c)
		{
			case '"':
				token = this->lexString();
				if (token.type != TOKEN_STRING && token.value.empty())
					throw JsonSyntaxException("Missing string end quote");
				break ;
			case '[':
				token = this->lexSymbol();
				break ;
			case ']':
				token = this->lexSymbol();
				break ;
			case '{':
				token = this->lexSymbol();
				break ;
			case '}':
				token = this->lexSymbol();
				break ;
			case ',':
				token = this->lexSymbol();
				break ;
			case ':':
				token = this->lexSymbol();
				break ;
			default:
				if (std::isalpha(c))
					token = this->lexKeyword();
				else if (std::isdigit(c) || c == '-')
					token = this->lexNumber();
				if (token.value.empty())
				{
					std::string	msg("Unexpected <");

					msg.append(1, this->peek());
					msg.append("> token.");
					throw JsonSyntaxException(msg);
				}
		}
		tokenList.push_back(token);
	}
	Token	endTok;

	endTok.type = TOKEN_END;
	tokenList.push_back(endTok);

	return tokenList;
}

Token	JsonLexer::lexNumber( void )
{
	Token	tok;

	tok.type = TOKEN_NUMBER;

	if (this->peek() == '-')
		tok.value.append(1, this->advance());

	while (!this->isEnd() && std::isdigit(this->peek()))
	{
		char	c = this->advance();

		tok.value.append(1, c); 
	}
	return tok;
}

Token	JsonLexer::lexKeyword( void )
{
	std::string	tmp;
	Token		tok;

	while (!this->isEnd() && std::isalpha(this->_input[this->_pos]))
		tmp.append(1, this->advance());
	if (tmp == "true" || tmp == "false")
	{
		tok.type = TOKEN_BOOL;
		tok.value = tmp;
	}
	else if (tmp == "null")
	{
		tok.type = TOKEN_NULL;
		tok.value = tmp;
	}
	else
		this->_pos -= 1;
	return tok;
}

Token	JsonLexer::lexSymbol( void )
{
	char	c = this->advance();
	Token	tok;

	switch (c)
	{
		case '[':
			tok.type = TOKEN_LBRACKET;
			tok.value.append("[");
			break ;
		case ']':
			tok.type = TOKEN_RBRACKET;
			tok.value.append("]");
			break ;
		case '{':
			tok.type = TOKEN_LBRACE;
			tok.value.append("{");
			break ;
		case '}':
			tok.type = TOKEN_RBRACE;
			tok.value.append("}");
			break ;
		case ':':
			tok.type = TOKEN_COLON;
			tok.value.append(":");
			break ;
		case ',':
			tok.type = TOKEN_COMMA;
			tok.value.append(",");
			break ;
	}
	return tok;
}

Token	JsonLexer::lexString( void )
{
	bool		quoted = false;
	std::size_t	start = this->_pos + 1;
	std::size_t	end;
	Token		tok;

	this->advance();
	if (this->peek() == '"')
	{
		tok.type = TOKEN_STRING;
		tok.value = std::string();
		this->advance();
		return tok;
	}
	while (this->_input[this->_pos])
	{
		char	c = this->advance();
		if (c == '"' && (this->_pos != 0 && this->_input[this->_pos - 1] != '\\'))
		{
			quoted = true;
			break ;
		}
		if (c == '\n')
			return tok;
	}
	if (quoted)
	{
		tok.type = TOKEN_STRING;	
		tok.value = this->_input.substr(start, this->_pos - 1 - start);
	}
	return tok;
}

// --- Exceptions
JsonLexer::JsonLexerException::JsonLexerException	( std::string msg )
{
	this->_msg = "JsonLexer Error: " + msg;
}
JsonLexer::JsonLexerException::~JsonLexerException	( void ) { }
const char	*JsonLexer::JsonLexerException::what( void ) const throw() { return this->_msg.c_str(); }

JsonLexer::JsonSyntaxException::JsonSyntaxException	( std::string msg )
{
	this->_msg = "Syntax Error: " + msg;
}
JsonLexer::JsonSyntaxException::~JsonSyntaxException	( void ) { }
const char	*JsonLexer::JsonSyntaxException::what( void ) const throw() { return this->_msg.c_str(); }

// --- DEBUG SECTION (REMOVE BEFORE PUSH)
std::string	JsonLexer::getSrc( void ) const { return this->_input; }

std::ostream	&operator<<	( std::ostream &out, const JsonLexer &lex )
{
	out << lex.getSrc();
	return out;
}

void	displayTokenList( std::vector<Token> &tokenList )
{
	if (tokenList.empty())
		return ;

	for (std::vector<Token>::iterator it = tokenList.begin(); it != tokenList.end(); it++)
	{
		switch (it->type)
		{
			case TOKEN_LBRACE:
				std::cout << "LBRACE";
				break ;
			case TOKEN_RBRACE:
				std::cout << "RBRACE";
				break ;
			case TOKEN_LBRACKET:
				std::cout << "LBRACKET";
				break ;
			case TOKEN_RBRACKET:
				std::cout << "RBRACKET";
				break ;
			case TOKEN_COLON:
				std::cout << "COLON";
				break ;
			case TOKEN_COMMA:
				std::cout << "COMMA";
				break ;
			case TOKEN_BOOL:
				std::cout << "BOOL";
				break ;
			case TOKEN_NULL:
				std::cout << "NULL";
				break ;
			case TOKEN_STRING:
				std::cout << "STRING";
				break ;
			case TOKEN_NUMBER:
				std::cout << "NUMBER";
				break ;
			case TOKEN_END:
				std::cout << "END";
				break ;
		}
		if (it != tokenList.end() - 1)
			std::cout << ", ";
	}
	std::cout << std::endl;
}

// void	displayTokenList( std::vector<Token> &tokenList )
// {
// 	if (tokenList.empty())
// 		return ;
//
// 	for (std::vector<Token>::iterator it = tokenList.begin(); it != tokenList.end(); it++)
// 	{
// 		switch (it->type)
// 		{
// 			case TOKEN_LBRACE:
// 				std::cout << "LBRACE: " << it->value;
// 				break ;
// 			case TOKEN_RBRACE:
// 				std::cout << "RBRACE: " << it->value;
// 				break ;
// 			case TOKEN_LBRACKET:
// 				std::cout << "LBRACKET: " << it->value;
// 				break ;
// 			case TOKEN_RBRACKET:
// 				std::cout << "RBRACKET: " << it->value;
// 				break ;
// 			case TOKEN_COLON:
// 				std::cout << "COLON: " << it->value;
// 				break ;
// 			case TOKEN_COMMA:
// 				std::cout << "COMMA: " << it->value;
// 				break ;
// 			case TOKEN_BOOL:
// 				std::cout << "BOOL: " << it->value;
// 				break ;
// 			case TOKEN_NULL:
// 				std::cout << "NULL: " << it->value;
// 				break ;
// 			case TOKEN_STRING:
// 				std::cout << "STRING: " << it->value;
// 				break ;
// 			case TOKEN_NUMBER:
// 				std::cout << "NUMBER: " << it->value;
// 				break ;
// 			case TOKEN_END:
// 				std::cout << "END";
// 				break ;
// 		}
// 		if (it != tokenList.end() - 1)
// 			std::cout << ", ";
// 	}
// 	std::cout << std::endl;
// }
