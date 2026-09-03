/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 17:38:00 by anfouger         ###   ########.fr       */
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
		//throw an exception
	}
	line.method = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		//throw an exception
	}
	line.target = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	line.version = strRequestLine.substr(0, 8);
	strRequestLine.erase(0, 8);
	if (strRequestLine != "\r\n")
	{
		// throw an exception
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
	if (method.empty() || (method != "GET" && method != "POST" && method != "DELETE"))
	{
		// throw an exception
	}
}

void		HttpParser::VerifyTarget(std::string target)
{
	if (target.empty() || target[0] != '/')
	{
		// throw an exception
	}
	if (target.size() > 8192)
	{
		// throw an exception (414 URI Too Long)
	}
	for (size_t i = 0; i < target.size(); ++i)
	{
		unsigned char c = target[i];
		if (c < 0x20 || c == 0x7F)
		{
			// throw an exception
		}
	}
	if (ContainsDotDotSegment(target))
	{
		// throw an exception (doesnt accept ".." in path to avoid attacks)
	}
}

bool HttpParser::ContainsDotDotSegment(const std::string& path)
{
	size_t pos = 0;
	while (pos < path.size())
	{
		size_t next = path.find('/', pos + 1);
		if (next == std::string::npos)
			next = path.size();
		
		std::string segment = path.substr(pos + 1, next - pos - 1);

		if (segment == "..")
			return true;

		pos = next;
	}
	return false;
}

void		HttpParser::VerifyVersion(std::string version)
{
	if (version.empty() || version != "HTTP/1.0")
	{
		// throw an exception
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
