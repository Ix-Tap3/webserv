/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tseche <tseche@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:27:42 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/09 19:53:33 by tseche           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include <iostream>
#include <cctype>
#include <sstream>
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
JsonLexer::JsonLexer	( std::string &filename ): _pos(0), _currentLine(1), _currentCol(1)
{
	if (filename.empty())
		throw JsonLexerException("Empty filename provided");
	if (!checkFileExtension(filename, "json"))
		throw JsonLexerException("Invalid file extension, please use only json files");

	std::ifstream	file(filename.c_str());

	if (!file.is_open())
		throw JsonLexerException("Cannot open file named: " + filename);
	if (file.peek() == std::ifstream::traits_type::eof())
		throw JsonLexerException("Invalid empty Json file provided");

	std::string	line;

	while (std::getline(file, line))
	{
		line.append("\n");
		this->_input.append(line);
	}
	if (this->_input[this->_input.length() - 2] == '\n')
		this->_input = this->_input.substr(0, this->_input.length() - 2);
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
		this->advance();
		if(this->peek() == '+' || this->peek() == '-')
			tok.value.append(1, this->advance());
		while (!this->isEnd() && std::isdigit(this->peek()))
			tok.value.append(1, this->advance());
	}
	setTokenPos(tok.pos);
	return tok;
}

Token	JsonLexer::lexKeyword( void )
{
	std::string	tmp;
	Token		tok;
	size_t i = 0;
	
	
	for (; i < this->_input.length() && std::isalpha(this->_input[this->_pos + i]); i++)
		tmp.append(1, this->_input[this->_pos + i]);
	if (tmp == "true" || tmp == "false")
	{
		tok.type = TOKEN_BOOL;
		tok.value = tmp;
		this->_pos += i;
	}
	else if (tmp == "null")
	{
		tok.type = TOKEN_NULL;
		tok.value = tmp;
		this->_pos += i;
	}
	setTokenPos(tok.pos);
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
	setTokenPos(tok.pos);
	return tok;
}

Token	JsonLexer::lexString( void )
{
	bool		quoted = false;
	std::size_t	start = this->_pos + 1;
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
	setTokenPos(tok.pos);
	return tok;
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


JsonLexer::JsonUnexpectedTokenException::JsonUnexpectedTokenException	( Token token )
{
	std::stringstream	ss;

	ss << "Syntax Error: Unexpected <'";
	ss << strTokenType(token.type) << "'> token at line " << token.pos.line << ", col " << token.pos.col;
	this->_msg = ss.str();
}

JsonLexer::JsonUnexpectedTokenException::JsonUnexpectedTokenException	( char value, std::size_t line, std::size_t col )
{
	std::stringstream	ss;
	std::string msg;

	if (value == '<' || value == '>')
		msg = std::string("'").append(1, value).append("'");
	else
		msg = value;		

	ss << "Syntax Error: Unexpected <" << msg << "> token at line " << line << ", col " << col;
	this->_msg = ss.str();
}

const char	*JsonLexer::JsonUnexpectedTokenException::what( void ) const throw() { return this->_msg.c_str(); }

JsonLexer::JsonUnexpectedTokenException::~JsonUnexpectedTokenException	( void ) throw() { }

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
