/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonValue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 11:37:31 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 14:22:12 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include <map>
# include <vector>
# include <ostream>
# include "JsonType.hpp"

class JsonValue
{
private:
	JsonType	_type;

	union
	{
		bool								boolValue;
		double								number;
		std::string							*str;
		std::vector<JsonValue>				*arr;
		std::map<std::string, JsonValue *>	*obj;
	};

public:
	JsonValue	( void );
	JsonValue	( int n );
	JsonValue	( float f );
	JsonValue	( bool value );
	JsonValue	( std::string &string );
	JsonValue	( std::vector<JsonValue> &array );
	JsonValue	( std::map<std::string, JsonValue *> &map );
	JsonValue	( const JsonValue &other );
	~JsonValue	( void );

	JsonValue	&operator=	( const JsonValue &other );

	JsonType	getType( void ) const;

	float								getFloat( void ) const;
	int									getInt( void ) const;
	bool								getBool( void ) const;
	std::string							*getString( void ) const;
	std::vector<JsonValue>				*getArray( void ) const;
	std::map<std::string, JsonValue *>	*getObject( void ) const;

	void	setValue( bool value );
	void	setValue( double number );
	void	setValue( std::string &str );
	void	setValue( std::vector<JsonValue> &arr );
	void	setValue( std::map<std::string, JsonValue *> &obj );
};

std::ostream	&operator<<	( std::ostream &out, const JsonValue &value );
