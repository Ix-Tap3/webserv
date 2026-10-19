/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tseche <tseche@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:27:42 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/14 15:43:21 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cctype>
#include <sstream>
#include <string>
#include "../../includes/json/JsonLexer.hpp"
#include "../../includes/json/JsonUnexpectedTokenException.hpp"

static bool	checkFileExtension( const std::string &filename, std::string extension)
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
JsonLexer::JsonLexer	( const std::string &filename ): _pos(0), _currentLine(1), _currentCol(1)
{
	if (filename.empty())
		throw JsonLexerException("Empty filename provided");
	if (!checkFileExtension(filename, "json"))
		throw JsonLexerException("Invalid file extension, please use only json files");

	std::ifstream	file(filename.c_str());

	if (!file.is_open())
		throw JsonLexerException("Cannot open file named: " + filename);

	std::stringstream	ss;
	ss << file.rdbuf();
	if (file.bad())
		throw JsonLexerException("Cannot read file: " + filename);
	this->_input = ss.str();
	if (this->_input.find_first_not_of(" \t\r\n") == std::string::npos)
		throw JsonLexerException("Invalid empty Json file provided");
}

// Member functions
void	JsonLexer::setTokenPos( TokenPos &pos ) const
{
	pos.line = this->_currentLine;
	pos.col = this->_currentCol;
}

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
	this->_currentCol++;
	return this->_input[idx];
}

void	JsonLexer::skipWhiteSpace( void )
{
	if (this->isEnd())
		return;
	while (!this->isEnd() && std::isspace(this->_input[this->_pos]))
	{
		if (this->peek() == '\n')
		{
			this->_currentLine++;
			this->_currentCol = 1;
		}
		else
			this->_currentCol++;
		this->_pos++;
	}
}

std::vector<Token>	JsonLexer::tokenize()
{
	std::vector<Token>	tokenList;

	while (!this->isEnd())
	{
		this->skipWhiteSpace();
		char	c = this->peek();
		Token	token;

		if (c == '\0')
			break;
		switch (c)
		{
			case '"':
				token = this->lexString();
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
					throw JsonUnexpectedTokenException(this->peek(), this->_currentLine, this->_currentCol );
		}

		tokenList.push_back(token);
	}
	Token	endTok;

	endTok.type = TOKEN_END;
	this->setTokenPos( endTok.pos );
	tokenList.push_back(endTok);

	return tokenList;
}

Token	JsonLexer::lexNumber( void )
{
	Token	tok;

	tok.type = TOKEN_NUMBER;
	setTokenPos(tok.pos);

	if (this->peek() == '-')
		tok.value.append(1, this->advance());

	while (!this->isEnd() && std::isdigit(this->peek()))
		tok.value.append(1, this->advance());
	if (this->peek() == '.')
		tok.value.append(1, this->advance());
	while (!this->isEnd() && std::isdigit(this->peek()))
		tok.value.append(1, this->advance());
	if (this->peek() == 'e' || this->peek() == 'E')
	{
		tok.value.append(1, this->advance());
		if(this->peek() == '+' || this->peek() == '-')
			tok.value.append(1, this->advance());
		while (!this->isEnd() && std::isdigit(this->peek()))
			tok.value.append(1, this->advance());
	}
	return tok;
}

Token	JsonLexer::lexKeyword( void )
{
	std::string	tmp;
	Token		tok;
	size_t i = 0;


	setTokenPos(tok.pos);
	for (; i < this->_input.length() && std::isalpha(this->_input[this->_pos + i]); i++)
	{
		tmp.append(1, this->_input[this->_pos + i]);
		this->_currentCol++;
	}
	if (tmp == "true" || tmp == "false")
	{
		tok.type = TOKEN_BOOL;
		tok.value = tmp;
		this->_pos += i;
		// if (tmp == "true")
		// 	this->_currentCol += 4;
		// else
		// 	this->_currentCol += 5;
	}
	else if (tmp == "null")
	{
		tok.type = TOKEN_NULL;
		tok.value = tmp;
		this->_pos += i;
		// this->_currentCol += 4;
	}
	return tok;
}

Token	JsonLexer::lexSymbol( void )
{
	Token	tok;
	setTokenPos(tok.pos);
	char	c = this->advance();

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

Token   JsonLexer::lexString( void )
{
	Token           tok;
	std::string     value;

	setTokenPos(tok.pos);
	this->advance();
	while (!this->isEnd())
	{
		char    c = this->advance();

		if (c == '"')
		{
			tok.type = TOKEN_STRING;
			tok.value = value;
			return tok;
		}
		if (c == '\n')
			throw JsonSyntaxException("Missing string end quote");
		if (static_cast<unsigned char>(c) < 0x20)
			throw JsonSyntaxException("Control character in string");
		if (c != '\\')
		{
			value += c;
			continue ;
		}
		if (this->isEnd())
			break ;
		c = this->advance();
		switch (c)
		{
			case '"':
				value += '"';
				break ;
			case '\\':
				value += '\\';
				break ;
			case '/':
				value += '/';
				break ;
			case 'b':
				value += '\b';
				break ;
			case 'f':
				value += '\f';
				break ;
			case 'n':
				value += '\n';
				break ;
			case 'r':
				value += '\r';
				break ;
			case 't':
				value += '\t';
				break ;
			case 'u':
				throw JsonSyntaxException("\\u escape sequences are not supported");
			default:
				throw JsonUnexpectedTokenException(c, this->_currentLine, this->_currentCol - 1);
		}
	}
	throw JsonSyntaxException("Missing string end quote");
}

// --- Exceptions
JsonLexer::JsonLexerException::JsonLexerException	( std::string msg )
{
	this->_msg = "JsonLexer Error: " + msg;
}
JsonLexer::JsonLexerException::~JsonLexerException	( void ) throw() { }
const char	*JsonLexer::JsonLexerException::what( void ) const throw() { return this->_msg.c_str(); }

JsonLexer::JsonSyntaxException::JsonSyntaxException	( std::string msg )
{
	this->_msg = "Syntax Error: " + msg;
}
JsonLexer::JsonSyntaxException::~JsonSyntaxException	( void ) throw() { }
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
