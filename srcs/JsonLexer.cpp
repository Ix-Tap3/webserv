/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonLexer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:27:42 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/26 17:57:29 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fstream>
#include "../includes/JsonLexer.hpp"

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
		throw JsonLexerException("Invalid empty file provided");

	std::string	line;

	while (std::getline(file, line))
		this->_input.append(line);
}

// Member functions

// --- Exceptions
JsonLexer::JsonLexerException::JsonLexerException	( std::string msg )
{
	this->_msg = "JsonLexer Error: " + msg;
}
JsonLexer::JsonLexerException::~JsonLexerException	( void ) { }

const char	*JsonLexer::JsonLexerException::what( void ) const throw() { return this->_msg.c_str(); }

// --- DEBUG SECTION (REMOVE BEFORE PUSH)
std::string	JsonLexer::getSrc( void ) const { return this->_input; }

std::ostream	&operator<<	( std::ostream &out, const JsonLexer &lex )
{
	out << lex.getSrc();
	return out;
}
