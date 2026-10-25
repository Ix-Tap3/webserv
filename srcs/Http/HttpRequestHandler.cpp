/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/25 15:22:25 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpRequestHandler.hpp>
#include <string>
#include <sys/stat.h>
#include <dirent.h>

// === INIT === //

HttpRequestHandler::HttpRequestHandler(): _config(NULL), _location(NULL)
{
	initTypeTable();
	this->_responseData._connection = 0;
}

HttpRequestHandler::HttpRequestHandler(const ServerConfig *config): _config(config), _location(NULL)
{
	initTypeTable();
	this->_responseData._connection = 0;
}

HttpRequestHandler::~HttpRequestHandler()
{
}

void	HttpRequestHandler::initTypeTable(void)
{
	this->_typeTable[".html"] = "text/html";
	this->_typeTable[".css"] = "text/css";
	this->_typeTable[".js"] = "application/javascript";
	this->_typeTable[".txt"] = "text/plain";
	this->_typeTable[".json"] = "application/json";
	this->_typeTable[".png"] = "image/png";
	this->_typeTable[".jpg"] = "image/jpeg";
	this->_typeTable[".gif"] = "image/gif";
	this->_typeTable[".svg"] = "image/svg+xml";
	this->_typeTable[".ico"] = "image/x-icon";
}

std::string HttpRequestHandler::getRequestTarget(std::string& target)
{
	size_t		q = target.find('?');
	std::string	query;

	if (q != std::string::npos)
	{
		query = target.substr(q + 1);
		target.erase(q);
	}
	return (query);
}

std::string HttpRequestHandler::getExtension(const std::string& target)
{
	std::string extension;

	size_t pos_extension = target.find_last_of('.');
	size_t pos_slash = target.find_last_of('/');
	if (pos_extension == std::string::npos
		|| (pos_slash != std::string::npos && pos_extension < pos_slash))
		return (extension);
	
	extension = target.substr(pos_extension, target.size() - 1);
	return (extension);
}

// === MAIN FUNCTION === //
ResponseData HttpRequestHandler::handleRequest(HttpRequest& httpRequest)
{
	this->_responseData = ResponseData();
	this->_location = NULL;
	if (this->_config == NULL)
		return (errorResponse(500));

	const std::string&	method = httpRequest.requestLine._method;
	std::string&		target = httpRequest.requestLine._target;

	getRequestTarget(target);
	if (target.find("..") != std::string::npos)
		return (errorResponse(403));

	this->_location = findLocation(target);
	if (this->_location == NULL)
		return (errorResponse(404));

	if (this->_location->redirect_code != -1)
	{
		this->_responseData._statusCode = this->_location->redirect_code;
		this->_responseData._location = this->_location->redirect_path;
		return (this->_responseData);
	}
	if (!isMethodAllowed(method))
		return (errorResponse(405));
	if (httpRequest._body.size() > static_cast<size_t>(this->_location->max_body_size))
		return (errorResponse(413));

	if (method == "GET")
		handleGet(httpRequest);
	else if (method == "POST")
		handlePost(httpRequest);
	else if (method == "DELETE")
		handleDelete(httpRequest);
	else
		this->_responseData._statusCode = 501;

	if (this->_responseData._statusCode >= 400)
		setErrorPage();
	return (this->_responseData);
}

ResponseData HttpRequestHandler::handleError(int statusCode)
{
	this->_responseData = ResponseData();
	this->_location = NULL;
	return (errorResponse(statusCode));
}

// === LOCATION === //
const LocationConfig	*HttpRequestHandler::findLocation(const std::string& target) const
{
	const LocationConfig	*best = NULL;

	for (std::vector<LocationConfig>::const_iterator it = this->_config->locations.begin();
		it != this->_config->locations.end(); ++it)
	{
		const std::string&	path = it->path;

		if (target.compare(0, path.size(), path) != 0)
			continue ;
		if (target.size() != path.size() && path[path.size() - 1] != '/'
			&& target[path.size()] != '/')
			continue ;
		if (best == NULL || path.size() > best->path.size())
			best = &(*it);
	}
	return (best);
}

int	HttpRequestHandler::getMaxContentLength( std::string target )
{
	getRequestTarget(target); 
	const LocationConfig	*location = findLocation(target);

	if (!location)
		return this->_config->max_body_size;
	return location->max_body_size;
}

bool	HttpRequestHandler::isMethodAllowed(const std::string& method) const
{
	for (std::vector<std::string>::const_iterator it = this->_location->methods.begin();
		it != this->_location->methods.end(); ++it)
	{
		if (*it == method)
			return (true);
	}
	return (false);
}

// === FILES === //
bool	HttpRequestHandler::readFile(const std::string& path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		return (false);

	this->_responseData._body.clear();
	char buf[1000];
	file.read(buf, sizeof(buf));
	std::streamsize n = file.gcount();
	while (n > 0)
	{
		this->_responseData._body.append(buf, n);
		file.read(buf, sizeof(buf));
		n = file.gcount();
	}
	this->_responseData._contentLength = this->_responseData._body.size();

	std::map<std::string, std::string>::iterator it = this->_typeTable.find(getExtension(path));
	if (it != this->_typeTable.end())
		this->_responseData._contentType = it->second;
	return (true);
}

void	HttpRequestHandler::setErrorPage(void)
{
	this->_responseData._body.clear();
	this->_responseData._contentLength = 0;
	this->_responseData._contentType.clear();
	if (this->_config == NULL)
		return ;

	const std::map<int, std::string>	*pages = &this->_config->error_pages;
	const std::string					*root = &this->_config->root;
	if (this->_location)
	{
		pages = &this->_location->error_pages;
		root = &this->_location->root;
	}
	std::map<int, std::string>::const_iterator it = pages->find(this->_responseData._statusCode);

	if (it == pages->end() || !readFile(*root + it->second))
	{
		this->_responseData._body.clear();
		this->_responseData._contentLength = 0;
	}
}

ResponseData	HttpRequestHandler::errorResponse(int statusCode)
{
	this->_responseData._statusCode = statusCode;
	setErrorPage();
	return (this->_responseData);
}

// === GET === //
void HttpRequestHandler::handleGet(HttpRequest& httpRequest)
{
	const std::string&	target = httpRequest.requestLine._target;
	std::string			path = this->_location->root + target;
	struct stat			st;

	if (access(path.c_str(), F_OK) == -1)
	{
		this->_responseData._statusCode = 404;
		return ;
	}
	else if (access(path.c_str(), R_OK) == -1 || stat(path.c_str(), &st) == -1)
	{
		this->_responseData._statusCode = 403;
		return ;
	}
	if (S_ISDIR(st.st_mode))
	{
		handleDirectory(target, path);
		return ;
	}
	if (!readFile(path))
	{
		this->_responseData._statusCode = 403;
		return ;
	}
	this->_responseData._statusCode = 200;
}

void HttpRequestHandler::handleDirectory(const std::string& target, const std::string& path)
{
	if (target[target.size() - 1] != '/')
	{
		this->_responseData._statusCode = 301;
		this->_responseData._location = target + "/";
		return ;
	}

	std::string	index = path.substr(0, path.size() - 1) + this->_location->default_page;
	if (access(index.c_str(), R_OK) == 0 && readFile(index))
	{
		this->_responseData._statusCode = 200;
		return ;
	}
	if (this->_location->directory_listing)
	{
		createListing(target, path);
		return ;
	}
	this->_responseData._statusCode = 403;
}

static std::string	htmlEscape(const std::string& str)
{
	std::string	res;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '&')
			res += "&amp;";
		else if (str[i] == '<')
			res += "&lt;";
		else if (str[i] == '>')
			res += "&gt;";
		else if (str[i] == '"')
			res += "&quot;";
		else
			res += str[i];
	}
	return (res);
}

void HttpRequestHandler::createListing(const std::string& target, const std::string& path)
{
	DIR	*dir = opendir(path.c_str());
	if (dir == NULL)
	{
		this->_responseData._statusCode = 403;
		return ;
	}

	std::string&	body = this->_responseData._body;
	std::string		title = "Index of " + htmlEscape(target);

	body = "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>" + title + "</title></head>\n";
	body += "<body>\n<h1>" + title + "</h1>\n<ul>\n";
	for (struct dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir))
	{
		std::string	name = entry->d_name;

		if (name == ".")
			continue ;
		if (entry->d_type == DT_DIR)
			name += "/";
		body += "<li><a href=\"" + htmlEscape(name) + "\">" + htmlEscape(name) + "</a></li>\n";
	}
	closedir(dir);
	body += "</ul>\n</body>\n</html>\n";

	this->_responseData._contentLength = body.size();
	this->_responseData._contentType = "text/html";
	this->_responseData._statusCode = 200;
}

// === POST === //
void HttpRequestHandler::handlePost(HttpRequest& httpRequest)
{
	(void)httpRequest;
}

// === DELETE === //
void HttpRequestHandler::handleDelete(HttpRequest& httpRequest)
{
	(void)httpRequest;
}
