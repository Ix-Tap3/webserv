/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:55:05 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpRequestHandler.hpp>

HttpRequestHandler::HttpRequestHandler()
{
}

HttpRequestHandler::~HttpRequestHandler()
{
}

ResponseData HttpRequestHandler::handleRequest(HttpRequest httpRequest)
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
}

// === GET === //
void HttpRequestHandler::handleGet(HttpRequest httpRequest)
{
	
}

// === POST === //
void HttpRequestHandler::handlePost(HttpRequest httpRequest)
{
	
}

// === DELETE === //
void HttpRequestHandler::handleDelete(HttpRequest httpRequest)
{
	
}
