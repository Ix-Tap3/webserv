/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 19:25:48 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpParser.hpp>

HttpParser::HttpParser()
{
}

HttpParser::~HttpParser()
{
}

RequestLine HttpParser::ParseRequestLine(std::string& strRequestLine)
{
	RequestLine line;
	
	size_t space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		throw HttpException(400, "Space separator not found in Request Line: " + strRequestLine);
	}
	line.method = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		throw HttpException(400, "Space separator not found in Request Line: " + strRequestLine);
	}
	line.target = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	line.version = strRequestLine.substr(0, 8);
	strRequestLine.erase(0, 8);
	if (strRequestLine != "\r\n")
	{
		throw HttpException(400, "End (\"\\r\\n\") not found in Request Line: " + strRequestLine);
	}

	VerifyRequestLine(line);
	return (line);
}

void	HttpParser::VerifyRequestLine(RequestLine line)
{
	VerifyMethod(line.method);
	VerifyTarget(line.target);
	VerifyVersion(line.version);
}

void		HttpParser::VerifyMethod(std::string method)
{
	if (method.empty())
	{
		throw HttpException(0, "Method is empty");
	}
	else if (method != "GET" && method != "POST" && method != "DELETE")
	{
		if (method == "PUT" || method == "HEAD" || method == "CONNECT" ||
			method == "OPTIONS" || method == "TRACE" || method == "PATCH")
		{
			throw HttpException(405, "Method not supported: " + method);
		}
		throw HttpException(400, "Unknown Method: " + method);
	}
}

void		HttpParser::VerifyTarget(std::string target)
{
	if (target.empty())
		throw HttpException(400, "Path is empty: " + target);

	std::string begin = target.substr(0, 7);
	if (target[0] != '/' && begin != "http://")
		throw HttpException(400, "Path isn't accepted: " + target);

	if (target.size() > 8192)
		throw HttpException(414, "URI Too Long");

	for (size_t i = 0; i < target.size(); ++i)
	{
		unsigned char c = target[i];
		if (c < 0x20 || c == 0x7F)
		{
			throw HttpException(400, "Invalid character in request target: " + target);
		}
	}
	// if (ContainsDotDotSegment(target))
	// {
	// 	// throw an exception (doesnt accept ".." in path to avoid attacks)
	// }
}

// bool HttpParser::ContainsDotDotSegment(const std::string& path)
// {
// 	size_t pos = 0;
// 	while (pos < path.size())
// 	{
// 		size_t next = path.find('/', pos + 1);
// 		if (next == std::string::npos)
// 			next = path.size();
		
// 		std::string segment = path.substr(pos + 1, next - pos - 1);

// 		if (segment == "..")
// 			return true;

// 		pos = next;
// 	}
// 	return false;
// }

void		HttpParser::VerifyVersion(std::string version)
{
	if (version.empty() || version != "HTTP/1.0")
	{
		throw HttpException(400, "Version isn't accepted: " + version);
	}	
}

void	HttpParser::DataSorting(std::string& header)
{
	size_t pos = 0;
	while (pos < header.size())
	{
		size_t eol = header.find("\r\n", pos);
		if (eol == std::string::npos)
		{
			eol = header.size();
		}
		
		std::string line = header.substr(pos, eol - pos);
		size_t	colon = line.find(':');
		
		if (pos == 0)
		{
			this->_HttpRequest._RequestLine = ParseRequestLine(line);
			pos = eol + 2;
			continue;
		}
		else if (colon != std::string::npos)
		{
			this->_HttpRequest._Header._HeadersFields.push_back(
				std::make_pair(line.substr(0, colon),
				line.substr(colon + 1)));
		}
		pos = eol + 2;
	}
}

Header	HttpParser::ParseHeader(std::string& header)
{
	if (header.empty())
		return ;
	
	DataSorting(header);
	
	return (this->_HttpRequest._Header);
}

Body	HttpParser::ParseBody(std::string&	body)
{
	
}
