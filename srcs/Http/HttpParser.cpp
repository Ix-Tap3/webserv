/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/16 16:10:29 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpParser.hpp>

// ============== //
// === BORING === //
// ============== //
HttpParser::HttpParser()
{
}

HttpParser::~HttpParser()
{
}

// ============= //
// === UTILS === //
// ============= //
std::vector<std::string> HttpParser::split(const std::string &s, char delim)
{
    std::vector<std::string> tokens;
    size_t start = 0;
    size_t end = s.find(delim);

    while (end != std::string::npos)
    {
        tokens.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delim, start);
    }
    tokens.push_back(s.substr(start));
    return tokens;
}

int	HttpParser::stringToInt(std::string str) const
{
	int					res;
	std::stringstream	ss;

	ss << str;
	ss >> res;

	return (res);
}

std::string	HttpParser::strToMin(std::string& str)
{
	std::string out(str);
	for (size_t i = 0; i < out.size(); ++i)
		out[i] = std::tolower(static_cast<unsigned char>(out[i]));
	return out;
}

bool	HttpParser::isTchar(char c)
{
	if (std::isalnum(static_cast<unsigned char>(c)))
		return true;

	static const std::string special = "!#$%&'*+-.^_`|~";
	return (special.find(c) != std::string::npos);
}

void	HttpParser::DeleteUselessSpace(std::string& str)
{
	if (str.empty())
		return ;
	for (size_t i = 0; i < str.length(); i++)
	{
		if (isspace(str[i]) != ' ' && i != 0)
		{
			str.erase(0, i);
			break;
		}
	}
	if (str.empty())
		return ;
	for (size_t i = str.length() - 1; i != 0; --i)
	{
		if (isspace(str[i]) && i != str.length() - 1)
		{
			str.erase(i + 1, str.length() - 1);
			break;
		}
	}
}

// ============== //
// === HEADER === //
// ============== //
Header	HttpParser::ParseHeader(std::string& header)
{
	if (header.empty())
	{
		throw HttpException(400, "Header empty");	
	}

	DataSorting(header);
	this->_httpRequest._requestLine = ParseRequestLine(this->_httpRequest._requestLine.raw_requestLine);
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		DeleteUselessSpace(it->second);
	}
	ParseHeaders();

	return (this->_httpRequest._header);
}

void	HttpParser::DataSorting(std::string& header)
{
	size_t pos = 0;
	while (pos < header.size())
	{
		size_t eol = header.find("\r\n", pos);
		if (eol == std::string::npos)
			throw HttpException(400, "Empty line in Header");
		
		std::string line = header.substr(pos, eol - pos);
		if (line.empty())
			break;
		
		if (pos == 0)
		{
			this->_httpRequest._requestLine.raw_requestLine = line;
			pos = eol + 2;
			continue;
		}
		else
		{
			size_t	colon = line.find(':');

			if (colon == std::string::npos)
				throw HttpException(400, "No colon found: " + line);

			this->_httpRequest._header._headersFields.push_back(
				std::make_pair(line.substr(0, colon),
				line.substr(colon + 1)));
		}
		pos = eol + 2;
	}
}

// ============ //
// Request Line //
// ============ //
RequestLine HttpParser::ParseRequestLine(std::string& strRequestLine)
{
	RequestLine res;
	
	size_t space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		throw HttpException(400, "Space separator not found in Request Line: " + strRequestLine);
	}
	res.method = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		throw HttpException(400, "Space separator not found in Request Line: " + strRequestLine);
	}
	res.target = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	res.version = strRequestLine.substr(0, 8);
	strRequestLine.erase(0, 8);
	if (strRequestLine != "\r\n")
	{
		throw HttpException(400, "End (\"\\r\\n\") not found in Request Line: " + strRequestLine);
	}

	VerifyRequestLine(res);
	return (res);
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
		throw HttpException(400, "Method is empty");
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

// ============== //
// Headers Fields //
// ============== //
void	HttpParser::ParseHeaders(void)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		if (it->first.empty())
			throw HttpException(400, "Header Fields name empty");
		
		VerifyHeaderName(it->first);
		VerifyHeaderValue(it->second);

		std::string name = strToMin(it->first);
		if (name == "content-length" || name == "connection" ||
			name == "content-type" || name == "host" || name == "transfer-encoding")
			VerifyKnownHeaders(it, name);
	}
}

void	HttpParser::VerifyHeaderName(std::string name)
{
	for (size_t i = 0; i < name.size(); ++i)
	{
		if (!isTchar(name[i]))
			throw HttpException(400, "Headers Name contains a non tchar:" + name);
	}
}

void	HttpParser::VerifyKnownHeaders(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	if (name == "content-length")
		VerifyContentLength(headerFields, name);
	else if (name == "connection")
		VerifyConnection(headerFields, name);
	else if (name == "transfer-encoding")
		VerifyTransferEncoding(headerFields, name);
	else if (name == "host")
		VerifyHost(headerFields, name);
	else if (name == "content-type")
		VerifyContentType(headerFields, name);
}

void		HttpParser::VerifyContentLength(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	if (headerFields->second.empty())
		throw HttpException(400, "Content-Length header fields value is empty");
	for (size_t i = 0; i < headerFields->second.length(); i++)
	{
		if (headerFields->second[i] < '0' || headerFields->second[i] > '9')
			throw HttpException(400, "Value of Content-Length header fields has to be a valid positive int or zero");
	}
	if (isWrongDupplicate(headerFields, name))
		throw HttpException(400, "Non authorize double headers appears twice" + headerFields->first);
	
}

void		HttpParser::VerifyConnection(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	
}

void		HttpParser::VerifyTransferEncoding(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	
}

void		HttpParser::VerifyHost(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	if (headerFields->second.empty())
		throw HttpException(400, "The value of this header cannot be empty: " + headerFields->first);
	if (isDupplicate(headerFields, name))
		throw HttpException(400, "Non authorize double headers appears twice: " + headerFields->first);
	if (!isValidHostname(headerFields->second) && !isValidIPv4(headerFields->second) && !isValidIPv6(headerFields->second))
		throw HttpException(400, "Value of Host Header is not acceptable: " + headerFields->second);
}

void		HttpParser::VerifyContentType(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	
}

bool		HttpParser::isWrongDupplicate(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::string it_name = strToMin(it->first);
		if (it != headerFields && it_name == name)
		{
			if (it->second == headerFields->second)
				continue;
			return (true);
		}
	}
	return (false);
}

bool		HttpParser::isDupplicate(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	for (std::vector<std::pair<std::string, std::string> >::iterator it =
		this->_httpRequest._header._headersFields.begin(); 
		it != this->_httpRequest._header._headersFields.end(); 
		++it)
	{
		std::string it_name = strToMin(it->first);
		if (it != headerFields && it_name == name)
			return (true);
	}
	return (false);
}

bool		HttpParser::isValidHostname(std::string& value)
{
	for (size_t i = 0; i < value.length(); i++)
	{
		if (i == 0 && !isalnum(value[i]))
			return (false);
		if (value[i] == '.')
		{
			if (i + 1 >= value.length() || i - 1 < 0) // is there chars around
				return (false);
			if (!isalnum(value[i + 1]) || !isalnum(value[i - 1])) // is the chars around aren't alphanum
				return (false);
		}
		if (value[i] == '-')
		{
			if (i + 1 >= value.length() || i - 1 < 0) // is there chars around
				return (false);
			if (value[i - 1] == '.' || value[i + 1] == '.')
				return (false);
		}
		if (value[i] == ':')
		{
			if (i + 1 >= value.length())
				return (false);
			if (!isValidPort(value, i + 1))
				return (false);
			else
				return (true);
		}
		if (!isValidCharHostname(value[i]))
			return (false);
	}
	return (true);
}

bool		HttpParser::isValidCharHostname(char c)
{
	if (!isalnum(c) && c != '.' && c != '-')
		return (false);
	return (true);
}

bool		HttpParser::isValidIPv4(std::string value)
{
	std::string ipv4;
	int count;

	size_t	beginPort = value.find(":");
	if (beginPort != std::string::npos)
	{
		if (!isValidPort(value, beginPort + 1))
			return (false);
		ipv4 = value.erase(beginPort);
	}
	else
		ipv4 = value;

	std::vector<std::string> ips = split(ipv4, '.');
	count = 0;
	for (std::vector<std::string>::iterator it = ips.begin(); it != ips.end(); it++)
	{
		for (size_t i = 0; i < it->length(); i++)
		{
			if (i > 3)
				return (false);
			if ((*it)[i] < '0' || (*it)[i] > '9')
				return (false);
		}
		int ipInt = stringToInt((*it));
		if (ipInt > 255 || ipInt < 0)
			return (false);
		count++;
	}
	if (count != 4)
		return (false);
	if (ipv4 == "255.255.255.255" || ipv4 == "0.0.0.0")
		throw HttpException(400, "This Ipv4 cannot be used cause its already reserved: " + value);
	return (true);
}

bool		HttpParser::isValidIPv6(std::string& value)
{
		
}

bool		HttpParser::isValidPort(std::string& value, int i)
{
	int count = 0;
	int	begin = i;

	while (i < value.length())
	{
		if (value[i] < '0' || value[i] > '9')
			return (false);
		i++;
		count++;
	}
	if (count > 5)
		return (false);	
	int portNb = stringToInt(value.substr(begin));
	if (portNb > 65535)
		return (false);
	return (true);
}

void	HttpParser::VerifyHeaderValue(std::string value)
{
	for (size_t i = 0; i < value.size(); ++i)
	{
		if (!isValidCharValue(value[i]))
			throw HttpException(400, "Headers value contains a non valid character:" + value);
	}
}

bool	HttpParser::isValidCharValue(char c)
{
	if ((c >= 0x00 && c <= 0x08) ||
		(c >= 0x0A && c <= 0x0F) || c == 0x7F)
	{
		return (false);
	}
	return (true);
}

Body	HttpParser::ParseBody(std::string&	body)
{
	
}
