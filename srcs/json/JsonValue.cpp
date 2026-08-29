/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 11:51:21 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 12:07:55 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/json/JsonValue.hpp"

JsonValue::JsonValue( JsonType	type ): _type(type) { }
JsonValue::JsonValue( const JsonValue &other )
{
	if (this != &other)
		*this = other;
}
JsonValue::~JsonValue( void )
{
	switch (this->_type)
	{
		case JSON_STRING:
			delete this->str;
			break ;
		case JSON_OBJECT:
			delete this->obj;
			break ;
		case JSON_ARRAY:
			delete this->arr;
			break ;
		default:
			break ;
	}
}

JsonValue	&JsonValue::operator=	( const JsonValue &other )
{
	if (this != &other)
	{
		this->_type = other._type;
		switch (this->_type)
		{
			case JSON_ARRAY:
				if (this->arr)
					delete this->arr;
				this->arr = other.arr;
				break ;
			case JSON_OBJECT:
				if (this->obj)
					delete this->obj;
				this->obj = other.obj;
				break ;
			case JSON_BOOL:
				this->boolValue = other.boolValue;
				break ;
			case JSON_NUMBER:
				this->number = other.number;
				break ;
			case JSON_STRING:
				if (this->str)
					delete this->str;
				this->str = other.str;
				break ;
			case JSON_NULL:
				break ;
		}
	}
	return *this;
}

void	JsonValue::setValue( bool value ) { this->boolValue = value; }
void	JsonValue::setValue( double number ) { this->number = number; }
void	JsonValue::setValue( std::string &str ) { this->str = &str; }
void	JsonValue::setValue( std::vector<JsonValue> &arr ) { this->arr = &arr; }
void	JsonValue::setValue( std::map<std::string, JsonValue *> &obj ) { this->obj = &obj; }
