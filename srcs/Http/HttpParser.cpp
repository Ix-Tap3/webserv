/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/17 18:42:23 by anfouger         ###   ########.fr       */
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
		throw HttpException(400, RED "Header empty" RESET);	
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
			throw HttpException(400, RED "Empty line in Header" RESET);
		
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
				throw HttpException(400, RED "No colon found: " YELLOW + line + RESET);

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
		throw HttpException(400, RED "Space separator not found in Request Line: " YELLOW + strRequestLine + RESET);
	}
	res.method = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	space = strRequestLine.find(' ');
	if (space == std::string::npos)
	{
		throw HttpException(400, RED "Space separator not found in Request Line: " YELLOW + strRequestLine + RESET);
	}
	res.target = strRequestLine.substr(0, space);
	strRequestLine.erase(0, space + 1);

	res.version = strRequestLine.substr(0, 8);
	strRequestLine.erase(0, 8);
	if (!strRequestLine.empty())
	{
		throw HttpException(400, RED "End (\"\\r\\n\") not found in Request Line: " YELLOW + strRequestLine + RESET);
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
		throw HttpException(400, RED "Method is empty" RESET);
	}
	else if (method != "GET" && method != "POST" && method != "DELETE")
	{
		if (method == "PUT" || method == "HEAD" || method == "CONNECT" ||
			method == "OPTIONS" || method == "TRACE" || method == "PATCH")
		{
			throw HttpException(405, RED "Method not supported: " YELLOW + method + RESET);
		}
		throw HttpException(400, RED "Unknown Method: " YELLOW + method + RESET);
	}
}

void		HttpParser::VerifyTarget(std::string target)
{
	if (target.empty())
		throw HttpException(400, RED "Path is empty: " YELLOW + target + RESET);

	std::string begin = target.substr(0, 7);
	if (target[0] != '/' && begin != "http://")
		throw HttpException(400, RED "Path isn't accepted: " YELLOW + target + RESET);

	if (target.size() > 8192)
		throw HttpException(414, RED "URI Too Long" RESET);

	for (size_t i = 0; i < target.size(); ++i)
	{
		unsigned char c = target[i];
		if (c < 0x20 || c == 0x7F)
		{
			throw HttpException(400, RED "Invalid character in request target: " YELLOW + target + RESET);
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
		throw HttpException(400, RESET "Version isn't accepted: " YELLOW + version + RESET);
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
			throw HttpException(400, RESET "Header Fields name empty" RESET);
		
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
			throw HttpException(400, RED "Headers Name contains a non tchar: " YELLOW + name + RESET);
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
		throw HttpException(400, RED "Content-Length header fields value is empty" RESET);
	for (size_t i = 0; i < headerFields->second.length(); i++)
	{
		if (headerFields->second[i] < '0' || headerFields->second[i] > '9')
			throw HttpException(400, RED "Value of Content-Length isn't a valid positive int or zero: " YELLOW +  headerFields->second + RESET);
	}
	if (isWrongDupplicate(headerFields, name))
		throw HttpException(400, RED "Non authorize double headers appears twice: " YELLOW + headerFields->first + RESET);
	
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
		throw HttpException(400, RED "The value of this header cannot be empty: " YELLOW + headerFields->first + RESET);
	
	if (isDupplicate(headerFields, name))
		throw HttpException(400, RED "Non authorize double headers appears twice: " YELLOW + headerFields->first + RESET);
	
	if (!headerFields->second.empty() && headerFields->second[0] == '[')
	{
		if (!isValidIPv6(headerFields->second))
			throw HttpException(400, RED "Value of Host (IPv6) isn't acceptable: " YELLOW + headerFields->second + RESET);	
	}
	else if (isPotentialIPv4(headerFields->second))
	{
		if (!isValidIPv4(headerFields->second))
			throw HttpException(400, RED "Value of Host (IPv4) isn't acceptable: " YELLOW + headerFields->second + RESET);		
	}
	else 
	{
		if (!isValidHostname(headerFields->second))
			throw HttpException(400, RED "Value of Host (hostname) isn't acceptable: " YELLOW + headerFields->second + RESET);		
	}
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
		if (i == 0 && !isalnum(static_cast<unsigned char>(value[i])))
			return (false);
		if (value[i] == '.')
		{
			if (i + 1 >= value.length() || i - 1 < 0) // is there chars around
				return (false);
			if (!isalnum(static_cast<unsigned char>(value[i + 1])) || !isalnum(static_cast<unsigned char>(value[i - 1]))) // is the chars around aren't alphanum
				return (false);
		}
		if (value[i] == '-') // all char are authorized around except for '.'
		{
			if (i + 1 >= value.length() || i - 1 < 0) // is there chars around
				return (false);
			if (value[i - 1] == '.' || value[i + 1] == '.')
				return (false);
		}
		if (value[i] == ':')
		{
			if (i + 1 >= value.length()) // is there something behind ':'
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
	if (!isalnum(static_cast<unsigned char>(c)) && c != '.' && c != '-')
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
	if (ipv4 == "255.255.255.255" ) // 0.0.0.0 is ok for a server
		throw HttpException(400, RED "This Ipv4 cannot be used cause its already reserved: " YELLOW + value + RESET);
	return (true);
}

bool		HttpParser::isPotentialIPv4(std::string value)
{
	for (size_t i = 0; i < value.length(); i++)
	{
		if (value[i] < '0' && value[i] > '9' && value[i] != '.')
			return (false);
	}
	return (true);
}

bool		HttpParser::isValidIPv6(std::string value)
{
	int endIP = 0;
	int group = 1;
	int	inGroup = 0;

	if (value.empty())
		return (false);
	
	if (value[0] != '[')
		return (false);
	size_t test = value.find("::");
	if (test == std::string::npos)
	{
		for (size_t i = 1; i < value.length(); i++)
		{
			endIP = i;

			if (value[i] == ':')
			{
				inGroup = 0;
				group++;
				if (group > 8) // max 8 groups when there's not any "::"
					return (false);
				continue;
			}
			
			if (value[i] == ']')
			{
				if (!isValidCharIPv6(value[i - 1]))
					return (false);
				break;	
			}
			
			if (!isValidCharIPv6(value[i]))
				return (false);
			
			inGroup++;
			if (inGroup > 4) // 4 char max between a ':'
				return (false);
		}
		if (group != 8) // if there's not "::" there should be excactly 8 groups
			return (false);
	}
	else
	{
		if ((test = value.find("::", test)) != std::string::npos) // is there a second "::"
			return (false);
		
		for (size_t i = 1; i < value.length(); i++)
		{
			endIP = i;

			if (value[i] == ':')
			{
				group++;
				inGroup = 0;
				if (group > 7) // max 7 groups when there's "::"
					return (false);
				continue;
			}

			if (value[i] == ']')
				break;
			
			if (!isValidCharIPv6(value[i]))
				return (false);
			
			inGroup++;
			if (inGroup > 4) // 4 char max between a ':'
				return (false);
		}
	}
	if (value[endIP] != ']') // if the ip isn't terminated by ']'
		return (false);
	endIP++;
	if (endIP < value.length() && value[endIP] == ':')
		return (isValidPort(value, endIP + 1));
	if (endIP < value.length())
		return (false);
	return (true);
}

bool		HttpParser::isValidCharIPv6(char c)
{
	if ((c >= '0' && c <= '9') ||
			c == 'a' || c == 'b' || c == 'c' ||
			c == 'd' || c == 'e' || c == 'f' ||
			c == 'A' || c == 'B' || c == 'C' ||
			c == 'D' || c == 'E' || c == 'F')
		return (true);
	else
		return (false);
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
			throw HttpException(400, RED "Headers value contains a non valid character:" YELLOW + value + RESET);
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
