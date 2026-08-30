/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 20:08:28 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/30 12:18:34 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <vector>
# include "JsonToken.hpp"
# include "JsonValue.hpp"

class JsonParser
{
private:
	std::vector<Token>	&_tokenList;
	std::size_t			_pos;

	Token		peek( void ) const;
	bool		check( TokenType type ) const;
	bool		isEnd( void ) const;
	Token		expect( TokenType type );
	Token		advance( void );
	JsonValue	parseValue( void );
	JsonValue	parseObject( void );
	JsonValue	parseArray( void );
	JsonValue	parseString( void );
	JsonValue	parseBool( void );
	JsonValue	parseNumber( void );
	JsonValue	parseNull( void );

	class JsonParseException:	public std::exception
	{
	private:
		std::string	_msg;
	
	public:
		JsonParseException	( std::string msg );
		~JsonParseException	( void ) throw();

		const char	*what( void ) const throw();
	};

public:
	JsonParser	( std::vector<Token> &tokenList );

	JsonValue	parse( void );
};

void	displayJsonTree( JsonValue &root );
