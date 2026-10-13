/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/13 22:35:45 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpRequestHandler.hpp>

// === INIT === //

HttpRequestHandler::HttpRequestHandler()
{
	initTypeTable();
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
	// else
		// Unknown method
}

// === GET === //
void HttpRequestHandler::handleGet(HttpRequest& httpRequest)
{
	std::string targetFile = httpRequest.requestLine._target;

	// Verify if i can access the target
	// Get the target and his information (type, size and target itself)
	// Send the responseData
	
	// std::ifstream file(targetFile, std::ios::binary);
}

// === POST === //
void HttpRequestHandler::handlePost(HttpRequest& httpRequest)
{
	
}

// === DELETE === //
void HttpRequestHandler::handleDelete(HttpRequest& httpRequest)
{
	
}
