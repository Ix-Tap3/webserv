/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonValue.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 11:37:31 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 12:06:03 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include <map>
# include <vector>
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
	JsonValue	( JsonType type );
	JsonValue	( const JsonValue &other );
	~JsonValue	( void );

	JsonValue	&operator=	( const JsonValue &other );

	void	setValue( bool value );
	void	setValue( double number );
	void	setValue( std::string &str );
	void	setValue( std::vector<JsonValue> &arr );
	void	setValue( std::map<std::string, JsonValue *> &obj );
};
