/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/18 16:15:29 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/05 19:15:31 by pcaplat          ###   ########.fr       */
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
		"redir",
		"upload",
		"path",
		"methods"
	};

	void		buildFields( void );
	void		buildPortsArray( std::vector<int> &portsArray, JsonArray &jsonArray ) const;
	void		buildHostsArray( std::vector<std::string> &hostArray, JsonArray &jsonArray ) const;
	void		buildErrorPagesMap( std::map<int, std::string> &errorPagesMap, JsonObj &jsonObj ) const;
	void		buildCgiMap( std::map<std::string, std::string> &cgiMap, JsonObj &jsonObj) const;
	void		buildRoot( std::string &input, std::string &root, std::string field ) const;
	void		buildDefaultPage( std::string &input, std::string &output, std::string field ) const;
	void		buildLocationPaths( std::string &input, std::string &output, std::string field ) const;
	void		buildRedir( JsonObj &input, std::string &pathOutput, int &codeOutput ) const;
	void		buildMethodsArray( JsonArray &value, std::vector<std::string> &array) const;
	void		parseServer( ServerConfig &config );
	void		fillServerConfig( ServerConfig &config ) const;
	void		parseLocations( std::vector<LocationConfig> &locConfig ) const;
	void		checkJsonType( JsonValue &value, std::string expected, std::string field ) const;
	bool		checkIpFormat( std::string ip ) const;
	bool		checkPath( std::string path ) const;
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

void	displayServerConfig( ServerConfig config );
