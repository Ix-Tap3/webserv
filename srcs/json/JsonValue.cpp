/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonValue.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 11:51:21 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 23:37:43 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <climits>
#include <exception>
#include <float.h>
#include <stdexcept>
#include <iostream>
#include "../../includes/json/JsonValue.hpp"

JsonValue::JsonValue	( void ): _type(JSON_NULL) { }
JsonValue::JsonValue	( int n ): _type(JSON_NUMBER) { this->number = static_cast<double>(n); }
JsonValue::JsonValue	( float n ): _type(JSON_NUMBER) { this->number = static_cast<double>(n); }
JsonValue::JsonValue	( double d): _type(JSON_NUMBER) { this->number = d; }
JsonValue::JsonValue	( bool value ): _type(JSON_BOOL) { this->boolValue = value; }
JsonValue::JsonValue	( std::string &string ): _type(JSON_STRING) { this->str = new std::string(string); }
JsonValue::JsonValue	( const char *string ): _type(JSON_STRING) { this->str = new std::string(string); }
JsonValue::JsonValue	( std::vector<JsonValue> &array ): _type(JSON_ARRAY) { this->arr = new std::vector<JsonValue>(array); }
JsonValue::JsonValue	( std::map<std::string, JsonValue> &map ): _type(JSON_OBJECT)
{ 
	this->obj = new std::map<std::string, JsonValue>(map); 
}
JsonValue::JsonValue	( const JsonValue &other ) { this->copyFrom(other); }

JsonValue::~JsonValue( void ) { this->clear(); }

JsonValue	&JsonValue::operator=	( const JsonValue &other )
{
	if (this != &other)
	{
		this->clear();
		this->copyFrom(other);
	}
	return *this;
}

JsonType							JsonValue::getType( void ) const { return this->_type; }
int									JsonValue::getInt( void ) const
{
	if (this->_type != JSON_NUMBER)
		throw std::logic_error("JsonValue is not an integer");
	if (this->number > INT_MAX || this->number < INT_MIN)
		throw std::logic_error("Impossible to get JsonValue INT value, number is out of limits");
	return static_cast<int>(this->number);
}
float								JsonValue::getFloat( void ) const
{
	if (this->_type != JSON_NUMBER)
		throw std::logic_error("JsonValue is not a floating number");
	if (this->number > FLT_MAX || this->number < -FLT_MAX)
		throw std::logic_error("Impossible to get JsonValue FLOAT value, number is out of limits");
	return static_cast<float>(this->number);
}
bool		JsonValue::getBool( void ) const
{
	if (this->_type != JSON_BOOL)
		throw std::logic_error("JsonValue is not a boolean");
	return this->boolValue;
}
std::string							*JsonValue::getString( void ) const
{
	if (this->_type != JSON_STRING)
		throw std::logic_error("JsonValue is not a string");
	return this->str;
}
std::vector<JsonValue>				*JsonValue::getArray( void ) const
{
	if (this->_type != JSON_ARRAY)
		throw std::logic_error("JsonValue is not an array");
	return this->arr;
}
std::map<std::string, JsonValue>	*JsonValue::getObject( void ) const
{
	if (this->_type != JSON_OBJECT)
		throw std::logic_error("JsonValue is not an object");
	return this->obj;
}

void	JsonValue::setValue( bool value ) { this->boolValue = value; }
void	JsonValue::setValue( double number ) { this->number = number; }
void	JsonValue::setValue( std::string *str ) { this->str = str; }
void	JsonValue::setValue( std::vector<JsonValue> *arr ) { this->arr = arr; }
void	JsonValue::setValue( std::map<std::string, JsonValue> *obj ) { this->obj = obj; }

void	JsonValue::clear( void )
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

void	JsonValue::copyFrom( const JsonValue &other )
{
	switch (other._type)
	{
		case JSON_STRING:
			this->_type = JSON_STRING;
			this->str = new std::string(*other.str);
			break ;
		case JSON_ARRAY:
			this->_type = JSON_ARRAY;
			this->arr = new std::vector<JsonValue>(*other.arr);
			break ;
		case JSON_OBJECT:
			this->_type = JSON_OBJECT;
			this->obj = new std::map<std::string, JsonValue>(*other.obj);
			break ;
		case JSON_NUMBER:
			this->_type = JSON_NUMBER;
			this->number = other.number;
			break ;
		case JSON_BOOL:
			this->_type = JSON_BOOL;
			this->boolValue = other.boolValue;
			break ;
		case JSON_NULL:
			this->_type = JSON_NULL;
	}
}

std::ostream	&operator<<	( std::ostream &out, const JsonValue &value )
{
	try
	{
		switch (value.getType())
		{
			case JSON_NUMBER:
				out << value.getFloat();
				break ;
			case JSON_BOOL:
				if (value.getBool())
					out << "true";
				else
					out << "false";
				break ;
			case JSON_NULL:
				out << "null";
				break ;
			case JSON_ARRAY:
				out << value.getArray();
				break ;
			case JSON_OBJECT:
				out << value.getObject();
				break ;
			case JSON_STRING:
				out << value.getString();
				break ;
		}
	} catch ( std::exception &e ) { out << e.what(); return out; }
	return out;
}
