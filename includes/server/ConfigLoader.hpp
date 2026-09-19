/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 16:15:29 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/19 17:02:49 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "configs.h"
# include "../json/JsonValue.hpp"

class ConfigLoader
{
private:
	JsonValue	_root;
	std::vector<JsonValue>	_locationSave;

	JsonValue		peek( void ) const;
	JsonValue		advance( void );
	bool			isEnd( void ) const;
	bool			expect( JsonType type ) const;
	std::string		checkMissingField( std::string src ) const;
	ServerConfig	parseObject( std::map<std::string, JsonValue> *obj );
	
	class ConfigException:	public std::exception
	{
	private:
		std::string	_msg;

	public:
		ConfigException		( std::string msg );
		~ConfigException	( void );

		const char	*what( void ) const throw();
	};

public:
	ConfigLoader	( JsonValue root );

	ServerConfig	load();
};
