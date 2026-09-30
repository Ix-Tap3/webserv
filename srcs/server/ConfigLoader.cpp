/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 16:14:33 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/30 11:26:29 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <sstream>
#include "../../includes/server/ConfigLoader.hpp"

// --- Constructor
ConfigLoader::ConfigLoader	( JsonValue root ): _root(root)
{
	if (this->_root.getType() != JSON_OBJECT)
		throw ConfigException("Config must start with a Json object");

	std::string	missings;

	missings = this->checkMissingField();
	if (!missings.empty())
		throw ConfigException("the following fields are missing in the configuration file: " + missings);

	std::cout << "field tests passed!" << std::endl;

	this->buildFields();

	std::cout << "fields build." << std::endl;

	for (ConfigFields::iterator it = this->_fields.begin(); it != _fields.end(); it++)
		std::cout << "key: " << it->first << " value: " << it->second << std::endl;
}

// --- Member functions
ServerConfig	ConfigLoader::load( void )
{
	ServerConfig	config;

	parseServer(&config);
	// parseLocations(&config.locations);

	return config;
}

void	ConfigLoader::parseServer( ServerConfig *config ) const
{
	JsonObj			*obj = this->_root.getObject();
	
	for (JsonObjIterator it = obj->begin(); it != obj->end(); it++)
	{
		std::cout << "it->first: " << it->first << ", type: " << typeToStr(it->second.getType()) << std::endl;
	}
}

void	ConfigLoader::buildFields( void )
{
	for (int i = 0; i != METHODS; i++)
		this->_fields[this->_fieldsName[i]] = static_cast<FieldsValue>(i);
}

std::string	ConfigLoader::checkMissingField( void ) const
{
	if (this->_root.getType() != JSON_OBJECT)
		throw ConfigException("Invalid format in configuration file.");

	JsonObj		*obj = this->_root.getObject();
	std::string	missings;

	if (!this->_root.contains("ports"))
		missings.append("ports");
	if (!this->_root.contains("hosts"))
	{
		if (!missings.empty())
			missings.append(", ");
		missings.append("hosts");
	}
	if (!this->_root.contains("locations"))
	{
		if (!missings.empty())
			missings.append(", ");
		missings.append("locations");
		return missings;
	}

	for (JsonObjIterator it = obj->begin(); it != obj->end(); it++)
	{
		if (it->first == "locations")
		{
			JsonArray	*arr = it->second.getArray();
			int			idx = 1;

			for (JsonArrIterator it2 = arr->begin(); it2 != arr->end(); it2++)
			{
				if (!it2->contains("path"))
				{
					std::stringstream	ss;

					ss << idx;
					if (!missings.empty())
						missings.append(", ");
					missings.append("path in locations field ");
					missings.append(ss.str());
				}
				idx++;
			}
		}
	}

	return missings;
}

// --- Exceptions
ConfigLoader::ConfigException::ConfigException	( std::string msg ): _msg("Config Error: ") { _msg.append(msg); }
ConfigLoader::ConfigException::~ConfigException	( void ) { }

const char	*ConfigLoader::ConfigException::what( void ) const throw() { return this->_msg.c_str(); }
