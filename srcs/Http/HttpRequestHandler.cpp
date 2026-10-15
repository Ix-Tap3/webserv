/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/14 20:13:22 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpRequestHandler.hpp>

// === INIT === //

HttpRequestHandler::HttpRequestHandler()
{
	initTypeTable();
	this->_responseData._connection = 0;
}

HttpRequestHandler::~HttpRequestHandler()
{
}

void	HttpRequestHandler::initTypeTable(void)
{
	this->_typeTable.insert(".html", "text/html");
	this->_typeTable.insert(".css", "text/css");
	this->_typeTable.insert(".js", "application/javascript");
	this->_typeTable.insert(".txt", "text/plain");
	this->_typeTable.insert(".json", "application/json");
	this->_typeTable.insert(".png", "image/png");
	this->_typeTable.insert(".jpg", "image/jpeg");
	this->_typeTable.insert(".gif", "image/gif");
	this->_typeTable.insert(".svg", "image/svg+xml");
	this->_typeTable.insert(".ico", "image/x-icon");
}

std::string HttpRequestHandler::getRequestTarget(std::string& target)
{
	std::vector<std::string> tmp = Utils::strSplit(target, '?');
	target = tmp[0];
	return (tmp[1]);
}

std::string HttpRequestHandler::getExtension(const std::string& target)
{
	std::string extension;

	size_t pos_extension = target.find_last_of('.');
	if (pos_extension == std::string::npos)
		return (extension);
	
	extension = target.substr(pos_extension, target.size() - 1);
	return (extension);
}

// === MAIN FUNCTION === //
ResponseData HttpRequestHandler::handleRequest(HttpRequest& httpRequest)
{
	const std::string& method = httpRequest.requestLine._method;

	if (method == "GET")
	{
		handleGet(httpRequest);
		return this->_responseData;
	}
	else if (method == "POST")
	{
		handlePost(httpRequest);
		return this->_responseData; 
	}
	else if (method == "DELETE")
	{
		handleDelete(httpRequest);
		return this->_responseData;
	}
	else
		return (this->_responseData);
}

// === GET === //
void HttpRequestHandler::handleGet(HttpRequest& httpRequest)
{
	std::string root = "/home/anfouger/Documents/5cercle/WebservCommonRepo/www/";
	// split the target and the query string if there is one
	std::string requestTarget = getRequestTarget(httpRequest.requestLine._target);
	std::string path = root + httpRequest.requestLine._target;

	// Verify if i can access the target
	if (access(path.c_str(), F_OK) == -1) // 404 Not Found (ENOENT)	
	{
		this->_responseData._statusCode = 404;
		return ;
	}
    else if (access(path.c_str(), R_OK) == -1) // 403 Forbidden (EACCES)
	{
		this->_responseData._statusCode = 403;
		return ;
	}	
	else // OK file exist and can be opened
	{
		// Get the target in binary
		std::ifstream file(path.c_str(), std::ios::binary);
		if (!file.is_open()) // 403 Forbidden (EACCES)
		{
			this->_responseData._statusCode = 403;
			return ;
		}

		char buf[1000];
		file.read(buf, sizeof(buf));
		std::streamsize n = file.gcount();
		while (n > 0)
		{
			this->_responseData._body.append(buf, n);
			file.read(buf, sizeof(buf));
			n = file.gcount();
		}
		
		// Get target size by the size of std::string body
		this->_responseData._contentLength = this->_responseData._body.size();

		// Get target type by substr the extension and compare it to typeTable
		std::string extension = getExtension(httpRequest.requestLine._target);
		std::map<std::string, std::string>::iterator it = this->_typeTable.find(extension);
		if (it != this->_typeTable.end())
			this->_responseData._contentType = it->second;

		// Set the status code
		this->_responseData._statusCode = 200;
	}
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
