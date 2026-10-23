/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 17:38:47 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/23 11:00:40 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpResponse.hpp>

HttpResponse::HttpResponse()
{
	initReasonPhraseTable();
}

void	HttpResponse::initReasonPhraseTable(void)
{
	// 2xx Success
	this->_reasonPhraseTable[200] = "OK";
	this->_reasonPhraseTable[201] = "Created";
	this->_reasonPhraseTable[202] = "Accepted";
	this->_reasonPhraseTable[204] = "No Content";

	// 3xx Redirection
	this->_reasonPhraseTable[301] = "Moved Permanently";
	this->_reasonPhraseTable[302] = "Moved Temporarily";
	this->_reasonPhraseTable[303] = "See Other";
	this->_reasonPhraseTable[304] = "Not Modified";
	this->_reasonPhraseTable[307] = "Temporary Redirect";
	this->_reasonPhraseTable[308] = "Permanent Redirect";

	// 4xx Client Error
	this->_reasonPhraseTable[400] = "Bad Request";
	this->_reasonPhraseTable[401] = "Unauthorized";
	this->_reasonPhraseTable[403] = "Forbidden";
	this->_reasonPhraseTable[404] = "Not Found";
	this->_reasonPhraseTable[405] = "Method Not Allowed";
	this->_reasonPhraseTable[408] = "Request Timeout";
	this->_reasonPhraseTable[413] = "Payload Too Large";

	// 5xx Server Error
	this->_reasonPhraseTable[500] = "Internal Server Error";
	this->_reasonPhraseTable[501] = "Not Implemented";
	this->_reasonPhraseTable[502] = "Bad Gateway";
	this->_reasonPhraseTable[503] = "Service Unavailable";
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::CreateHttpResponse(const ResponseData& responseData)
{
	std::string response;

	response.append(createHeader(responseData));
	response.append(responseData._body);

	return (response);
}

std::string	HttpResponse::createHeader(const ResponseData& responseData)
{
	std::string header;

	header.append(createStatusLine(responseData));
	header.append(createHeaderFields(responseData));
	header.append("\r\n");

	return (header);
}

std::string	HttpResponse::createHeaderFields(const ResponseData& responseData)
{
	std::string	headerFields;

	if (!responseData._location.empty())
	{
		headerFields.append("Location: ");
		headerFields.append(responseData._location);
		headerFields.append("\r\n");
	}

	headerFields.append("Content-Length: ");
	headerFields.append(Utils::intToString(responseData._contentLength));
	headerFields.append("\r\n");

	headerFields.append("Content-Type: ");
	headerFields.append(responseData._contentType);
	headerFields.append("\r\n");

	headerFields.append("Connection: ");
	if (responseData._connection)
		headerFields.append("keep-alive");
	else
		headerFields.append("close");
	headerFields.append("\r\n");

	return (headerFields);
}

std::string	HttpResponse::createStatusLine(const ResponseData& responseData)
{
	std::string statusLine = "HTTP/1.0 "; // basic initialisation for all

	statusLine.append(Utils::intToString(responseData._statusCode)); // put the status code in the status line
	
	std::map<int, std::string>::const_iterator it = this->_reasonPhraseTable.find(responseData._statusCode);
	if (it != this->_reasonPhraseTable.end())
	{
		// Known status code
		statusLine.append(" ");
		statusLine.append(it->second);
		statusLine.append("\r\n");
		return (statusLine);
	}
	// Unknown status code
	statusLine.append(" ");
	statusLine.append("Unknown code");
	statusLine.append("\r\n");
	return (statusLine);
}
