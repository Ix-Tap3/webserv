/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 17:02:56 by anfouger         ###   ########.fr       */
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

	if (!strRequestLine.empty())
	{
		//throw an exception
	}

	VerifyRequestLine(line);
	return (line);
}

void	HttpParser::VerifyRequestLine(RequestLine requestLine)
{
	
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
