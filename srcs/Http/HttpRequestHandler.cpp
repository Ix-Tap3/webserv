/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:19:30 by anfouger         ###   ########.fr       */
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
	const std::string& method = httpRequest._requestLine.method;

    if (method == "GET")
        return handleGet(httpRequest);
    else if (method == "POST")
        return handlePost(httpRequest);
    else if (method == "DELETE")
        return handleDelete(httpRequest);
}

// === GET === //
ResponseData HttpRequestHandler::handleGet(HttpRequest httpRequest)
{
	
}

// === POST === //
ResponseData HttpRequestHandler::handlePost(HttpRequest httpRequest)
{
	
}

// === DELETE === //
ResponseData HttpRequestHandler::handleDelete(HttpRequest httpRequest)
{
	
}
