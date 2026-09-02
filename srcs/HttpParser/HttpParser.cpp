/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/02 19:54:45 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpParser.hpp>

HttpParser::HttpParser()
{
}

HttpParser::~HttpParser()
{
}

RequestLine HttpParser::ParseRequestLine(std::string strRequestLine)
{
	RequestLine line;
	
	line.method = " ";
	line.target = " ";
	line.version = " ";
	return (line);
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
			continue;
		}
		else if (colon != std::string::npos)
		{
			this->_HttpRequest._Header._HeadersFields.push_back(
				std::make_pair(line.substr(0, colon),
				line.substr(colon + 1, eol)));
		}
		pos = eol + 2;
	}
}

Header	HttpParser::ParseHeader(std::string& header)
{
	// if (header.empty())
	// 	return ;
	
	DataSorting(header);
	
	return (this->_HttpRequest._Header);
}




Body	HttpParser::ParseBody(std::string&	body)
{
	
}
