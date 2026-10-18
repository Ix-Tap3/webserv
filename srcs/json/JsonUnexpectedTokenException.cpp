/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonUnexpectedTokenException.cpp                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 12:02:34 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/14 15:34:37 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/json/JsonUnexpectedTokenException.hpp"
#include <sstream>


JsonUnexpectedTokenException::JsonUnexpectedTokenException	( Token token, ErrorType error )
{
	std::stringstream	ss;
	std::string			src;

	switch (error)
	{
		case SYNTAX_ERROR:
			src = "Syntax Error: ";
			break ;
		case PARSE_ERROR:
			src = "Parse Error: ";
			break ;
	}
	ss << src;
	ss << "Unexpected <'";
	ss << strTokenType(token.type) << "'> token at line " << token.pos.line << ", col " << token.pos.col;
	this->_msg = ss.str();
}

JsonUnexpectedTokenException::JsonUnexpectedTokenException	( Token token, TokenType expect, ErrorType error )
{
	std::stringstream	ss;
	std::string			src;

	switch (error)
	{
		case SYNTAX_ERROR:
			src = "Syntax Error: ";
			break ;
		case PARSE_ERROR:
			src = "Parse Error: ";
			break ;
	}
	ss << src;
	ss << "Unexpected <'";
	ss << strTokenType(token.type) << "'> token at line " << token.pos.line << ", col " << token.pos.col;
	ss << ". Expected: " << strTokenType(expect);
	this->_msg = ss.str();
}

JsonUnexpectedTokenException::JsonUnexpectedTokenException	( char value, std::size_t line, std::size_t col )
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

const char	*JsonUnexpectedTokenException::what( void ) const throw() { return this->_msg.c_str(); }

JsonUnexpectedTokenException::~JsonUnexpectedTokenException	( void ) throw() { }
