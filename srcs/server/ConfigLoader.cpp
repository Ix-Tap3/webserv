/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 16:14:33 by pcaplat           #+#    #+#             */
/*   Updated: 2026/09/30 17:36:42 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cmath>
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

	std::cout << std::endl;
}

// --- Member functions
ServerConfig	ConfigLoader::load( void )
{
	ServerConfig	config;

	parseServer(config);
	// parseLocations(config.locations);

	std::cout << "server ip: " << config.ip << std::endl;
	std::cout << "server root directory: " << config.root << std::endl;
	std::cout << "server default_page: " << config.default_page << std::endl;
	std::cout << "server ports: ";
	for (std::vector<int>::iterator it = config.ports.begin(); it != config.ports.end(); it++)
	{
		std::cout << *it;
		if (it + 1 != config.ports.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
	std::cout << "server hosts: ";
	for (std::vector<std::string>::iterator it = config.hosts.begin(); it != config.hosts.end(); it++)
	{
		std::cout << *it;
		if (it + 1 != config.hosts.end())
			std::cout << ", ";
	}
	std::cout << std::endl;

	return config;
}

void	ConfigLoader::parseServer( ServerConfig &config )
{
	JsonObj			*obj = this->_root.getObject();
	
	for (JsonObjIterator it = obj->begin(); it != obj->end(); it++)
	{
		ConfigFields::iterator	field = this->_fields.find(it->first);

		if (field == this->_fields.end())
			throw ConfigException("Unknowned " + it->first + " field in configuration file");

		switch (field->second)
		{
			case LOCATIONS:
				this->_locationsPos = it;
				break ;
			case IP:
				if (!this->checkJsonType(it->second, "string"))
					throw ConfigException("\"ip\" field must be a Json string");
				if (!this->checkIpFormat(*(it->second.getString())))
					throw ConfigException("Invalid IP format, please use only IPV4 adresses (eg: 127.0.0.1)");

				config.ip = *(it->second.getString());
				break ;
			case ROOT:
				if (!this->checkJsonType(it->second, "string"))
					throw ConfigException("\"root\" field must be a Json string");
				if (it->second.getString()->find_first_of(".") != std::string::npos)
					throw ConfigException("\"root\" field path link to a file, not a directory");
				if (!this->checkPath(*(it->second.getString())))
					throw ConfigException("Invalid path provided in server \"root\" field, please use only absolute path");

				config.root = *(it->second.getString());
				break ;
			case DEFAULT_PAGE:
				if (!this->checkJsonType(it->second, "string"))
					throw ConfigException("\"default_page\" field must be a Json string");
				if (it->second.getString()->find_first_of(".") == std::string::npos)
					throw ConfigException("\"default_page\" field path link to a directory, not a file");
				if (!this->checkPath(*(it->second.getString())))
					throw ConfigException("Invalid path provided in server \"default_page\" field, please use only absolute path");

				config.default_page = *it->second.getString();
				break ;
			case PORTS:
				if (!this->checkJsonType(it->second, "array"))
					throw ConfigException("\"ports\" field must be a Json array");
				
				this->buildPortsArray(config.ports, *it->second.getArray());
				break ;
			case HOSTS:
				if (!this->checkJsonType(it->second, "array"))
					throw ConfigException("\"hosts\" field must be a Json array");

				this->buildHostsArray(config.hosts, *it->second.getArray());
				break ;
			default:
				break ;
		}
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

bool	ConfigLoader::checkJsonType( JsonValue &value, std::string expected ) const
{
	switch (value.getType())
	{
		case JSON_STRING:
			if (expected != "string" )
				return false;
			break ;
		case JSON_ARRAY:
			if (expected != "array")
				return false;
			break ;
		case JSON_BOOL:
			if (expected != "bool")
				return false;
			break;
		case JSON_NULL:
			if (expected != "null")
				return false;
			break ;
		case JSON_NUMBER:
			if (expected != "number")
				return false;
			break ;
		case JSON_OBJECT:
			if (expected != "object")
				return false;
			break ;
	}
	return true;
}

bool	ConfigLoader::checkIpFormat( std::string ip ) const
{
	if (ip.find_first_not_of("0123456789.") != std::string::npos)
		return false;

	int						digits = 0;
	int						sections = 1;

	for (std::string::iterator it = ip.begin(); it != ip.end(); it++)
	{
		if (*it == '.' && it + 1 == ip.end())
			return false;

		if (*it == '.' || it + 1 == ip.end())
		{
			if (digits == 0 || digits > 3)
				return false;
			if (digits > 1 && ip.at(0) == 0)
				return false;

			if (it + 1 == ip.end())
				digits++;

			std::string			sub = ip.substr(0, digits);
			std::stringstream	ss;
			int					nb;

			ss << sub;
			ss >> nb;
			
			if (nb > 255)
				return false;

			if (it + 1 != ip.end())
			{
				ip = ip.substr(digits + 1, std::string::npos);
				it = ip.begin();
				sections++;
			}

			digits = 0;
		}
		digits++;
	}

	if (sections != 4)
		return false;

	return true;
}

bool	ConfigLoader::checkPath( std::string path ) const
{
	if (path.empty())
		return false;

	if (path.find('\0') != std::string::npos)
		return false;

	if (path[0] != '/')
		return false;

	if (path.find("..") != std::string::npos)
		return false;

	return true;
}

void	ConfigLoader::buildPortsArray( std::vector<int> &portsArray, JsonArray &jsonArray ) const
{
	if (jsonArray.empty())
		throw ConfigException("Empty \"ports\" field, please insert at least one port");
	for (JsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); it++)
	{
		if (!this->checkJsonType(*it, "number"))
			throw ConfigException("Invalid Port provided. Please use only integers between 1 and 65 535");

		double	iptr;
		double	rest = std::modf(it->getDouble(), &iptr);

		if (rest != 0.0)
			throw ConfigException("Invalid floating number in \"ports\" field, ports array must be fill with only integer between 1 and 65 535.");

		int	port = it->getInt();

		if (port <= 0 || port > 65535)
			throw ConfigException("Invalid port range in \"ports\" field, please use only integers between 1 and 65 535");

		portsArray.push_back(port);
	}
}

void	ConfigLoader::buildHostsArray( std::vector<std::string> &hostsArray, JsonArray &jsonArray ) const
{
	if (jsonArray.empty())
		throw ConfigException("Empty \"hosts\" field, please insert at least one host");

	for (JsonArray::iterator it = jsonArray.begin(); it != jsonArray.end(); it++)
	{
		if (!this->checkJsonType(*it, "string"))
			throw ConfigException("Invalid hostname provided. hostnames must be a Json string");

		std::string	*hostname = it->getString();
		std::size_t	dot = hostname->find_last_of('.');

		if (dot == std::string::npos)
			throw ConfigException("Hostnames must have a dot followed by a domain extension");

		hostsArray.push_back(*hostname);
	}
}

// --- Exceptions
ConfigLoader::ConfigException::ConfigException	( std::string msg ): _msg("Config Error: ") { _msg.append(msg); }
ConfigLoader::ConfigException::~ConfigException	( void ) { }

const char	*ConfigLoader::ConfigException::what( void ) const throw() { return this->_msg.c_str(); }
