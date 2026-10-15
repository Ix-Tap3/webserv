/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 17:38:47 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/15 18:28:42 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpResponse.hpp>

HttpResponse::HttpResponse()
{
	initReasonPhraseTable();
}

void	HttpResponse::initReasonPhraseTable(void)
{
	// 1xx Informational
	this->_reasonPhraseTable[100] = "Continue";
	this->_reasonPhraseTable[101] = "Switching Protocols";

	// 2xx Success
	this->_reasonPhraseTable[200] = "OK";
	this->_reasonPhraseTable[201] = "Created";
	this->_reasonPhraseTable[202] = "Accepted";
	this->_reasonPhraseTable[203] = "Non-Authoritative Information";
	this->_reasonPhraseTable[204] = "No Content";
	this->_reasonPhraseTable[205] = "Reset Content";
	this->_reasonPhraseTable[206] = "Partial Content";

	// 3xx Redirection
	this->_reasonPhraseTable[300] = "Multiple Choices";
	this->_reasonPhraseTable[301] = "Moved Permanently";
	this->_reasonPhraseTable[302] = "Found";
	this->_reasonPhraseTable[303] = "See Other";
	this->_reasonPhraseTable[304] = "Not Modified";
	this->_reasonPhraseTable[305] = "Use Proxy";
	this->_reasonPhraseTable[307] = "Temporary Redirect";
	this->_reasonPhraseTable[308] = "Permanent Redirect";

	// 4xx Client Error
	this->_reasonPhraseTable[400] = "Bad Request";
	this->_reasonPhraseTable[401] = "Unauthorized";
	this->_reasonPhraseTable[402] = "Payment Required";
	this->_reasonPhraseTable[403] = "Forbidden";
	this->_reasonPhraseTable[404] = "Not Found";
	this->_reasonPhraseTable[405] = "Method Not Allowed";
	this->_reasonPhraseTable[406] = "Not Acceptable";
	this->_reasonPhraseTable[407] = "Proxy Authentication Required";
	this->_reasonPhraseTable[408] = "Request Timeout";
	this->_reasonPhraseTable[409] = "Conflict";
	this->_reasonPhraseTable[410] = "Gone";
	this->_reasonPhraseTable[411] = "Length Required";
	this->_reasonPhraseTable[412] = "Precondition Failed";
	this->_reasonPhraseTable[413] = "Payload Too Large";
	this->_reasonPhraseTable[414] = "URI Too Long";
	this->_reasonPhraseTable[415] = "Unsupported Media Type";
	this->_reasonPhraseTable[416] = "Range Not Satisfiable";
	this->_reasonPhraseTable[417] = "Expectation Failed";
	this->_reasonPhraseTable[426] = "Upgrade Required";

	// 5xx Server Error
	this->_reasonPhraseTable[500] = "Internal Server Error";
	this->_reasonPhraseTable[501] = "Not Implemented";
	this->_reasonPhraseTable[502] = "Bad Gateway";
	this->_reasonPhraseTable[503] = "Service Unavailable";
	this->_reasonPhraseTable[504] = "Gateway Timeout";
	this->_reasonPhraseTable[505] = "HTTP Version Not Supported";
}

HttpResponse::~HttpResponse()
{
}

std::string HttpResponse::CreateHttpResponse(const ResponseData& responseData)
{	
	this->_response.append(createStatusLine(responseData));
}

std::string	HttpResponse::createStatusLine(const ResponseData& responseData)
{
	std::string statusLine = "HTTP/1.0 "; // basic initialisation for all

	statusLine.append(Utils::intToString(responseData._statusCode)); // put the status code in the status line
	
	std::map<int, std::string>::const_iterator it = this->_reasonPhraseTable.find(responseData._statusCode);
	if (it != this->_reasonPhraseTable.end())
	{
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
