/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 16:14:33 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/19 17:09:23 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "../../includes/server/ConfigLoader.hpp"

ConfigLoader::ConfigLoader	( JsonValue root ): _root(root) { }

ServerConfig	ConfigLoader::load( void )
{
	ServerConfig	config;

	if (this->_root.getType() != JSON_OBJECT)
		throw ConfigException("Config must start with a Json object");

	std::string	missing = this->checkMissingField("server");
	if (!missing.empty())
		throw ConfigException(missing + " field is missing in the server configuration");

	std::cout << "field tests passed!" << std::endl;
	// config = this->parseObject(this->_root.getObject());

	return config;
}

std::string	ConfigLoader::checkMissingField( std::string src ) const
{
	if (src == "server")
	{
		if (!this->_root.contains("ports"))
			return "ports";
		else if (!this->_root.contains("locations"))
			return "locations";
		else if (!this->_root.contains("hosts"))
			return "hosts";
	}
	else if (src == "locations")
	{
		if (!this->_root.contains("path"))
			return "path";
	}
	return "";
}

// --- Exceptions
ConfigLoader::ConfigException::ConfigException	( std::string msg ): _msg("Config Error: ") { _msg.append(msg); }
ConfigLoader::ConfigException::~ConfigException	( void ) { }

const char	*ConfigLoader::ConfigException::what( void ) const throw() { return this->_msg.c_str(); }
