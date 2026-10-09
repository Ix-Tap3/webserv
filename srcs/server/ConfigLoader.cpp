/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfigLoader.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tseche <tseche@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/19 16:14:33 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/09 20:08:14 by tseche           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <climits>
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
	this->init_fieldname();
	this->buildFields();
}

// --- Member functions
ServerConfig	ConfigLoader::load( void )
{
	ServerConfig	config;

	config.listing_set = false;
	config.max_body_size = -1;
	parseServer(config);
	fillServerConfig(config);
	parseLocations(config.locations);
	fillLocationsConfig(config);
	// displayServerConfig(config);

	return config;
}

void ConfigLoader::init_fieldname(){
	this->_fieldsName.push_back("ip");
	this->_fieldsName.push_back("root");
	this->_fieldsName.push_back("default_page");
	this->_fieldsName.push_back("ports");
	this->_fieldsName.push_back("hosts");
	this->_fieldsName.push_back("locations");
	this->_fieldsName.push_back("cgis");
	this->_fieldsName.push_back("error_pages");
	this->_fieldsName.push_back("max_body_size");
	this->_fieldsName.push_back("directory_listing");
	this->_fieldsName.push_back("redir");
	this->_fieldsName.push_back("upload");
	this->_fieldsName.push_back("path");
	this->_fieldsName.push_back("methods");
}

void	ConfigLoader::buildRoot( std::string &input, std::string &root, std::string field ) const
{
	std::string	tmp;

	if (field == "locations")
		tmp = "in locations field";
	if (input.find_first_of(".") != std::string::npos)
		throw ConfigException("\"root\" path " + tmp + " link to a file, not a directory");
	if (!checkPath(input))
	{
		if (field == "server")
			tmp = "in server field";
		throw ConfigException("Invalid \"root\" path provided " + tmp + ", please use only absolute path");
	}

	root = input;
}

void	ConfigLoader::buildDefaultPage( std::string &input, std::string &output, std::string field ) const
{
	std::string	tmp;

	if (field == "locations")
		tmp = "in locations field";
	if (input.find_first_of(".") == std::string::npos)
		throw ConfigException("\"default_page\" path " + tmp + " link to a directory, not a file");
	if (!checkPath(input))
	{
		if (field == "server")
			tmp = "in server field";
		throw ConfigException("Invalid \"default_page\" path provided " + tmp + ", please use only absolute path");
	}
	output = input;
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
				checkJsonType(it->second, "string", "ip");

				if (!this->checkIpFormat(*(it->second.getString())))
					throw ConfigException("Invalid IP format, please use only IPV4 adresses (eg: 127.0.0.1) or localhost");

				config.ip = *(it->second.getString());
				break ;
			case ROOT:
				checkJsonType(it->second, "string", "root");
				buildRoot(*it->second.getString(), config.root, "server");
				break ;
			case DEFAULT_PAGE:
				checkJsonType(it->second, "string", "default_page");
				buildDefaultPage(*it->second.getString(), config.default_page, "server");
				break ;
			case PORTS:
				checkJsonType(it->second, "array", "ports");
				
				this->buildPortsArray(config.ports, *it->second.getArray());
				break ;
			case HOSTS:
				checkJsonType(it->second, "array", "hosts");

				this->buildHostsArray(config.hosts, *it->second.getArray());
				break ;
			case CGIS:
				checkJsonType(it->second, "object", "cgis");

				this->buildCgiMap(config.cgis, *it->second.getObject());
				break ;
			case ERROR_PAGES:
				checkJsonType(it->second, "object", "error_pages");
				buildErrorPagesMap(config.error_pages, *it->second.getObject());
				break ;
			case MAX_BODY_SIZE:
				checkJsonType(it->second, "number", "max_body_size");

				config.max_body_size = it->second.getInt();
				break ;
			case DIRECTORY_LISTING:
				checkJsonType(it->second, "bool", "directory_listing");

				config.directory_listing = it->second.getBool();
				config.listing_set = true;
				break ;
			default:
				break ;
		}
	}
}

void	ConfigLoader::buildLocationPaths( std::string &input, std::string &output, std::string field ) const
{
	if (input.find_first_of(".") != std::string::npos)
		throw ConfigException("\"" + field + "\" in locations field link to a file, not a directory");
	if (!checkPath(input))
	{
		std::string	tmp;

		if (field == "upload")
			tmp = " path";
		throw ConfigException("Invalid \"" + field + "\" " + tmp + " provided in locations field, please use only absolute path");
	}

	output = input;
}

void	ConfigLoader::parseLocations( std::vector<LocationConfig> &locations) const
{
	checkJsonType(this->_locationsPos->second, "array", "locations");

	JsonArray	arr = *this->_locationsPos->second.getArray();

	if (arr.empty())
		throw ConfigException("\"locations\" field is an emty array. Please fill it with at leat one location.");

	for (JsonArray::iterator itArr = arr.begin(); itArr != arr.end(); itArr++)
	{
		LocationConfig	config;

		config.max_body_size = -1;
		config.listing_set = false;
		config.redirect_code = -1;

		if (itArr->getType() != JSON_OBJECT)
			throw ConfigException("Entries in \"locations\" field must be Json objects");

		JsonObj	obj = *itArr->getObject();

		for (JsonObj::iterator itObj = obj.begin(); itObj != obj.end(); itObj++)
		{
			ConfigFields::const_iterator	field = this->_fields.find(itObj->first);

			if (field == this->_fields.end())
				throw ConfigException("Unknowned " + itObj->first + " field in configuration file");

			switch (field->second)
			{
				case MAX_BODY_SIZE:
					checkJsonType(itObj->second, "number", "max_body_size");

					config.max_body_size = itObj->second.getInt();
					break ;
				case ERROR_PAGES:
					checkJsonType(itObj->second, "object", "error_pages");
					buildErrorPagesMap(config.error_pages, *itObj->second.getObject());
					break ;
				case ROOT:
					checkJsonType(itObj->second, "string", "root");
					buildRoot(*itObj->second.getString(), config.root, "server");
					break ;
				case DEFAULT_PAGE:
					checkJsonType(itObj->second, "string", "default_page");
					buildDefaultPage(*itObj->second.getString(), config.default_page, "server");
					break ;
				case DIRECTORY_LISTING:
					checkJsonType(itObj->second, "bool", "directory_listing");

					config.directory_listing = itObj->second.getBool();
					config.listing_set = true;
					break ;
				case UPLOAD:
					checkJsonType(itObj->second, "string", "upload");
					buildLocationPaths(*itObj->second.getString(), config.upload, "upload");
					break ;
				case PATH:
					checkJsonType(itObj->second, "string", "upload");
					buildLocationPaths(*itObj->second.getString(), config.path, "path");
					break ;
				case METHODS:
					checkJsonType(itObj->second, "array", "methods");
					buildMethodsArray(*itObj->second.getArray(), config.methods);
					break ;
				case REDIR:
					if (itObj->second.getType() != JSON_OBJECT && itObj->second.getType() != JSON_NULL)
						throw ConfigException("\"redir\" field must be a Json Object or \"null\"");
					if (itObj->second.getType() == JSON_NULL)
						break ;
					buildRedir(*itObj->second.getObject(), config.redirect_path, config.redirect_code);
					break ;
				default:
					break ;
			}
		}

		// fillLocationConfig(config);
		locations.push_back(config);
	}
}

void	ConfigLoader::buildRedir( JsonObj &input, std::string &pathOutput, int &codeOutput ) const
{
	if (input.size() != 1)
		throw ConfigException("Invalid numbers of redirections in locations field. Only one redirection per location is allowed by the server");

	JsonObj::iterator	redirIt = input.begin();

	if (redirIt->first.find_first_not_of("0123456789") != std::string::npos || redirIt->second.getType() != JSON_STRING)
		throw ConfigException("Invalid redirection format: \"redir\" field must be formated in the following format: \"REDIR_CODE\": \"REDIR_PATH\"");

	double	res;
	char	*endptr;

	res = std::strtod(redirIt->first.c_str(), &endptr);
	if (res == HUGE_VAL || res == -HUGE_VAL || res > INT_MAX || res < INT_MIN)
		throw ConfigException("Int overflow in \"redir\" in locations field");
	codeOutput = static_cast<int>(res);

	if (redirIt->second.getString()->find_first_of('.') != std::string::npos)
		throw ConfigException("\"redirection path\" in locations field link to a directory, not a file");
	if (!checkPath(*redirIt->second.getString()))
		throw ConfigException("Invalid redirection path provided in locations field");
	pathOutput = *redirIt->second.getString();

}

void	ConfigLoader::buildMethodsArray( JsonArray &value, std::vector<std::string> &array) const
{
	if (value.empty())
	{
		array.push_back("GET");
		return ;
	}

	for (JsonArray::iterator it = value.begin(); it != value.end(); it++)
	{
		checkJsonType(*it, "string", "methods");

		std::string	tmp = *it->getString();

		if ( tmp != "GET" && tmp != "POST" && tmp != "DELETE")
			throw ConfigException("Invalid method " + tmp + " provided in locations field, this server support the following methods : GET, POST, DELETE");

		array.push_back(tmp);
	}
}

void	ConfigLoader::fillServerConfig( ServerConfig &config ) const
{
	if (config.ip.empty() || config.ip == "localhost")
		config.ip = "127.0.0.1";
	if (config.default_page.empty())
		config.default_page = "/index.html";
	if (config.error_pages.empty())
		config.error_pages[505] = "/errors/505.html";
	if (config.max_body_size == -1)
		config.max_body_size = 10000;
	if (config.listing_set == false)
		config.directory_listing = false;
}

void	ConfigLoader::fillLocationsConfig( ServerConfig &config ) const
{
	for (std::vector<LocationConfig>::iterator it = config.locations.begin(); it != config.locations.end(); it++)
	{
		if (it->max_body_size == -1)
			it->max_body_size = config.max_body_size;
		if (it->redirect_code == -1 && !it->redirect_path.empty())
			throw ConfigException("Redirections must have a redirection code associated");
		if (it->listing_set == false)
			it->directory_listing = config.directory_listing;
		if (it->root.empty())
			it->root = config.root;
		if (it->default_page.empty())
			it->default_page = config.default_page;
		if (it->methods.empty())
			it->methods.push_back("GET");
		if (it->error_pages.empty())
			it->error_pages = config.error_pages;
	}
}

void	ConfigLoader::buildFields( void )
{
	for (int i = 0; i <= METHODS; i++)
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
	if (!this->_root.contains("root"))
	{
		if (!missings.empty())
			missings.append(", ");
		missings.append("root");
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

void	ConfigLoader::checkJsonType( JsonValue &value, std::string expected, std::string field ) const
{
	bool	isValid = true;
	
	switch (value.getType())
	{
		case JSON_STRING:
			if (expected != "string" )
				isValid = false;
			break ;
		case JSON_ARRAY:
			if (expected != "array")
				isValid = false;
			break ;
		case JSON_BOOL:
			if (expected != "bool")
				isValid = false;
			break;
		case JSON_NULL:
			if (expected != "null")
				isValid = false;
			break ;
		case JSON_NUMBER:
			if (expected != "number")
				isValid = false;
			break ;
		case JSON_OBJECT:
			if (expected != "object")
				isValid = false;
			break ;
	}
	if (!isValid)
		throw ConfigException("\"" + field + "\" field must be a Json " + expected);
}

bool	ConfigLoader::checkIpFormat( std::string ip ) const
{
	if (ip == "localhost")
		return true;
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
		if (it->getType() != JSON_NUMBER)
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
		checkJsonType(*it, "string", "host");

		std::string	*hostname = it->getString();
		std::size_t	dot = hostname->find_last_of('.');

		if (dot == std::string::npos && *hostname != "localhost")
			throw ConfigException("Hostnames must have a dot followed by a domain extension. Or use \"localhost\"");

		hostsArray.push_back(*hostname);
	}
}

void	ConfigLoader::buildCgiMap( std::map<std::string, std::string> &cgiMap, JsonObj &jsonObj ) const
{
	if (jsonObj.empty())
		return ;

	for (JsonObjIterator it = jsonObj.begin(); it != jsonObj.end(); it++)
	{
		if (it->first.at(0) != '.')
			throw ConfigException("Keys in \"cgis\" field must be a file extension and start with a dot");
		if (it->second.getType() != JSON_STRING)
			throw ConfigException("Invalid Cgis Format, please use Json string as key and Json string as value in \"cgis\" field");
		
		std::string	path = *it->second.getString();

		if (!this->checkPath( path ))
			throw ConfigException("Invalid path provided in \"cgis\" field at key \"" + it->first + "\"");

		cgiMap[it->first] = path;
	}
}

void	ConfigLoader::buildErrorPagesMap( std::map<int, std::string> &errorPasgesMap, JsonObj &jsonObj ) const
{
	if (jsonObj.empty())
		return ;

	for (JsonObjIterator it = jsonObj.begin(); it != jsonObj.end(); it++)
	{
		if (it->first.find_first_not_of("0123456789") != std::string::npos)
			throw ConfigException("Keys in \"error_pages\" field must be a Json String and must be composed only by digits");

		if (it->first[0] == '0')
			throw ConfigException("Keys in \"error_pages\" cannot start with '0'");

		double	code;
		char	*endptr;

		code = std::strtod(it->first.c_str(), &endptr);
		if (code == HUGE_VAL || code == -HUGE_VAL || code > INT_MAX || code < INT_MIN)
			throw ConfigException("Keys in \"error_pages\" field must be a valid Integers");

		checkJsonType(it->second, "string", "error_pages");

		std::string	path = *it->second.getString();

		if (!this->checkPath(path))
			throw ConfigException("Invalid Path provided in \"error_pages\" field at key \"" + it->first + "\". Use only absolute path");

		int	key = static_cast<int>(code);

		errorPasgesMap[key] = path;
	}
}

void	displayServerConfig( ServerConfig config )
{
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
	std::cout << "server cgis: ";
	for (std::map<std::string, std::string>::iterator it = config.cgis.begin(); it != config.cgis.end(); it++)
	{
		std::cout << "[" << it->first << ", " << it->second << "]";
		std::map<std::string, std::string>::iterator cpy = it;
		if (cpy++ != config.cgis.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
	std::cout << "server error_pages: ";
	for (std::map<int, std::string>::iterator it = config.error_pages.begin(); it != config.error_pages.end(); it++)
	{
		std::cout << "[" << it->first << ", " << it->second << "]";
		std::map<int, std::string>::iterator cpy = it;
		if (cpy++ != config.error_pages.end())
			std::cout << ", ";
	}
	std::cout << std::endl;
	std::cout << "server max_body_size: " << config.max_body_size << std::endl;
	std::cout << "server directory_listing: " << (config.directory_listing ? "true" : "false") << std::endl;
	
	std::cout << "server locations: " << std::endl;
	int	idx = 1;
	for (std::vector<LocationConfig>::iterator it = config.locations.begin(); it != config.locations.end(); it++)
	{
		std::cout << "\tLocation " << idx << " max_body_size: " << it->max_body_size << std::endl;
		if (!it->redirect_path.empty())
			std::cout << "\tLocation " << idx << " redir : code: " << it->redirect_code << ", path: " << it->redirect_path << std::endl;
		else
			std::cout << "\tNo redirection set for this location." << std::endl;
		std::cout << "\tLocation " << idx << " directory_listing: " << (it->directory_listing ? "true" : "false") << std::endl;
		std::cout << "\tLocation " << idx << " max_body_size: " << it->max_body_size << std::endl;
		std::cout << "\tLocation " << idx << " root: " << it->root << std::endl;
		std::cout << "\tLocation " << idx << " default_page: " << it->default_page << std::endl;
		std::cout << "\tLocation " << idx << " upload storage: " << it->upload << std::endl;
		std::cout << "\tLocation " << idx << " path: " << it->path << std::endl;
		std::cout << "\tLocation " << idx << " methods: ";
		for (std::vector<std::string>::iterator itMeth = it->methods.begin(); itMeth != it->methods.end(); itMeth++)
		{
			std::cout << *itMeth;
			if (itMeth + 1 != it->methods.end())
				std::cout << ", ";
		}
		std::cout << std::endl;
		std::cout << "\tLocation " << idx << " error_pages: ";
		for (std::map<int, std::string>::iterator itErr = it->error_pages.begin(); itErr != it->error_pages.end(); itErr++)
		{
			std::cout << "[" << itErr->first << ", " << itErr->second << "]";
			std::map<int, std::string>::iterator cpy = itErr;
			if (cpy++ != it->error_pages.end())
				std::cout << ", ";
		}
		idx++;
	}
}

// --- Exceptions
ConfigLoader::ConfigException::ConfigException	( std::string msg ): _msg("Config Error: ") { _msg.append(msg); }
ConfigLoader::ConfigException::~ConfigException	( void ) throw() { }

const char	*ConfigLoader::ConfigException::what( void ) const throw() { return this->_msg.c_str(); }
