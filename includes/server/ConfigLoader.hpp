/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 16:15:29 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/30 11:13:02 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "configs.h"
# include "../json/JsonValue.hpp"

typedef std::map<std::string, FieldsValue>	ConfigFields;

class ConfigLoader
{
private:
	JsonValue		_root;
	JsonObjIterator	_locationsPos;
	ConfigFields	_fields;
	const std::vector<std::string>	_fieldsName= {
		"ip",
		"root",
		"default_page",
		"ports",
		"hosts",
		"locations",
		"cgis",
		"error_pages",
		"max_body_size",
		"directory_listing",
		"redir_code",
		"redir_path",
		"upload",
		"path",
		"methods"
	};

	void		buildFields( void );
	void		parseServer( ServerConfig *config ) const;
	void		parseLocations( std::vector<LocationConfig> *locConfig ) const;
	bool		checkIpFormat( std::string ip ) const;
	std::string	checkMissingField( void ) const;
	
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
