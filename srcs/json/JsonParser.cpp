/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 20:22:45 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/30 13:50:20 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include <cstdlib>
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

static std::string	getUnexpectedTokenError(const Token &token, std::size_t pos)
{
	std::stringstream	ss;

	ss << "Unexpected " << strTokenType(token.type) << " token, value: <" << token.value << "> at index: " << pos;

	return ss.str();
}

Token	JsonParser::expect( TokenType type )
{
	Token	current = this->peek();

	if (current.type != type)
	{
		std::string	msg = getUnexpectedTokenError(current, this->_pos);

		msg += ", expected " + strTokenType(type) + " token";
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
	JsonValue	root = this->parseValue();
	this->expect(TOKEN_END);
	return root;
}

JsonValue	JsonParser::parseValue( void )
{
	Token	current;

	current = this->peek();
	switch (current.type)
	{
		case TOKEN_LBRACE:
			return this->parseObject();
		case TOKEN_LBRACKET:
			return this->parseArray();
		case TOKEN_STRING:
			return this->parseString();
		case TOKEN_NUMBER:
			return this->parseNumber();
		case TOKEN_BOOL:
			return this->parseBool();
		case TOKEN_NULL:
			return this->parseNull();
		default:
			throw JsonParseException(getUnexpectedTokenError(current, this->_pos));
	}
}

JsonValue	JsonParser::parseObject( void )
{
	this->expect(TOKEN_LBRACE);

	bool								empty = false;
	std::map<std::string, JsonValue>	map;

	if (this->check(TOKEN_RBRACE))
		empty = true;
	while (true && !empty)
	{
		std::string	key = expect(TOKEN_STRING).value;
		expect(TOKEN_COLON);
		if (map.count(key) > 0)
			throw JsonParseException("Invalid duplicated key " + key );
		map[key] = this->parseValue();
		if (!this->check(TOKEN_COMMA))
			break ;
		this->advance();
	}
	this->expect(TOKEN_RBRACE);
	return JsonValue(map);
}

JsonValue	JsonParser::parseArray( void )
{
	this->expect(TOKEN_LBRACKET);

	bool					empty = false;
	std::vector<JsonValue>	arr;

	if (this->check(TOKEN_RBRACKET))
		empty = true;
	while (true && !empty)
	{
		JsonValue	value = this->parseValue();

		arr.push_back(value);
		if (!this->check(TOKEN_COMMA))
			break ;
		this->advance();
	}
	this->expect(TOKEN_RBRACKET);
	return JsonValue(arr);
}

JsonValue	JsonParser::parseNumber( void )
{
	Token	tok = this->expect(TOKEN_NUMBER);

	const char	*str = tok.value.c_str();
	char		*end;
	double		res = std::strtod(str, &end);

	if (end == str || *end != '\0')
		throw	JsonParseException("Invalid number format: " + tok.value);

	return JsonValue(res);
}

JsonValue	JsonParser::parseString( void )
{
	std::string	str(this->expect(TOKEN_STRING).value);

	return JsonValue(str);
}

JsonValue	JsonParser::parseBool( void )
{
	Token	tok = this->expect(TOKEN_BOOL);

	if (tok.value == "true")
		return JsonValue(true);
	return JsonValue(false);
}

JsonValue	JsonParser::parseNull( void )
{
	this->expect(TOKEN_NULL);

	return JsonValue();
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

// --- Debug section Remove before push
static void	printIndent( int depth )
{
    for (int i = 0; i < depth; i++)
        std::cout << "  ";
}

static void	debugJsonTree( const JsonValue &value, int depth )
{
    switch (value.getType())
    {
        case JSON_OBJECT:
        {
            std::cout << "{" << std::endl;
            std::map<std::string, JsonValue> *obj = value.getObject();
            std::map<std::string, JsonValue>::iterator it = obj->begin();
            std::map<std::string, JsonValue>::iterator end = obj->end();
            while (it != end)
            {
                printIndent(depth + 1);
                std::cout << "\"" << it->first << "\": ";
                debugJsonTree(it->second, depth + 1);
                ++it;
                if (it != end)
                    std::cout << ",";
                std::cout << std::endl;
            }
            printIndent(depth);
            std::cout << "}";
            break ;
        }
        case JSON_ARRAY:
        {
            std::cout << "[" << std::endl;
            std::vector<JsonValue> *arr = value.getArray();
            for (std::size_t i = 0; i < arr->size(); i++)
            {
                printIndent(depth + 1);
                debugJsonTree((*arr)[i], depth + 1);
                if (i + 1 < arr->size())
                    std::cout << ",";
                std::cout << std::endl;
            }
            printIndent(depth);
            std::cout << "]";
            break ;
        }
        case JSON_STRING:
            std::cout << "\"" << *value.getString() << "\"";
            break ;
        case JSON_NUMBER:
            std::cout << value.getFloat();
            break ;
        case JSON_BOOL:
            std::cout << (value.getBool() ? "true" : "false");
            break ;
        case JSON_NULL:
            std::cout << "null";
            break ;
    }
}

void	displayJsonTree( JsonValue &root )
{
    debugJsonTree(root, 0);
    std::cout << std::endl;
}
