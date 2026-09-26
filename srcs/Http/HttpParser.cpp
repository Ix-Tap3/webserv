/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:12:24 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/26 16:03:50 by anfouger         ###   ########.fr       */
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
	size_t start = 0;
	while (start < str.length() && std::isspace(static_cast<unsigned char>(str[start])))
		start++;

	if (start == str.length())
	{
		str.clear();
		return ;
	}

	size_t end = str.length() - 1;
	while (end > start && std::isspace(static_cast<unsigned char>(str[end])))
		end--;

	str = str.substr(start, end - start + 1);
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
}

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
		throw HttpException(501, RED "The header \"Transfer-Encoding\" isn't implemented" RESET);
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
	std::vector<std::string> value = split(headerFields->second, ',');
	for (std::vector<std::string>::iterator it = value.begin(); it != value.end() ; it++)
	{
		std::string cpy = (*it);
		DeleteUselessSpace(cpy);
		if (cpy.empty())
			throw HttpException(400, RED "Empty value (Connection Header):" YELLOW + headerFields->first + headerFields->second + RESET);
		for (size_t i = 0; i < cpy.length(); i++)
		{
			if (std::isspace(cpy[i]))
				throw HttpException(400, RED "White space in middle of a value (Connection Header): " YELLOW + headerFields->second + RESET);	
		}	
	}
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
void HttpParser::VerifyContentType(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name)
{
	std::string value = headerFields->second;
	DeleteUselessSpace(value);

	if (value.empty())
		throw HttpException(400, "Empty Content-Type");

	size_t semi = value.find(';');
	std::string mediaType = value.substr(0, semi);
	DeleteUselessSpace(mediaType);

	size_t slash = mediaType.find('/');

	if (slash == std::string::npos)
		throw HttpException(400, "Invalid Content-Type: missing '/'");

	if (slash == 0 || slash == mediaType.length() - 1)
		throw HttpException(400, "Invalid Content-Type");

	std::string type = mediaType.substr(0, slash);
	std::string subtype = mediaType.substr(slash + 1);

	if (!isValidToken(type) || !isValidToken(subtype))
		throw HttpException(400, "Invalid Content-Type");

	while (semi != std::string::npos)
	{
		size_t begin = semi + 1;
		size_t next = value.find(';', begin);

		std::string parameter = value.substr(begin, next - begin);
		DeleteUselessSpace(parameter);

		if (parameter.empty())
			throw HttpException(400, "Empty Content-Type parameter");

		size_t equal = parameter.find('=');

		if (equal == std::string::npos ||
			equal == 0 ||
			equal == parameter.length() - 1)
			throw HttpException(400, "Invalid Content-Type parameter");

		std::string paramName = parameter.substr(0, equal);
		std::string paramValue = parameter.substr(equal + 1);

		DeleteUselessSpace(paramName);
		DeleteUselessSpace(paramValue);

		if (!isValidToken(paramName) || paramValue.empty())
			throw HttpException(400, "Invalid Content-Type parameter");

		semi = next;
	}
}

bool HttpParser::isValidToken(const std::string& str)
{
    if (str.empty())
        return false;

    const std::string separators = "()<>@,;:\\\"/[]?={} \t";

    for (size_t i = 0; i < str.length(); ++i)
    {
        unsigned char c = static_cast<unsigned char>(str[i]);

        if (c < 0x21 || c > 0x7E)
            return false;

        if (separators.find(c) != std::string::npos)
            return false;
    }

    return true;
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

bool		HttpParser::isPotentialIPv4(std::string& value)
{
	for (size_t i = 0; i < value.length(); i++)
	{
		if (!((value[i] >= '0' && value[i] <= '9') || value[i] == '.' || value[i] == ':'))
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
	
	size_t tripleColon = value.find(":::");
	if (tripleColon != std::string::npos)
		return (false); 

	size_t doubleColon = value.find("::");
	if (doubleColon == std::string::npos)
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
		std::cout << doubleColon;
		doubleColon = value.find("::", doubleColon + 1);
		if (doubleColon != std::string::npos) // is there a second "::"
		{
			std::cout << "flag1" << doubleColon;
			return (false);	
		}
		
		for (size_t i = 1; i < value.length(); i++)
		{
			endIP = i;

			if (value[i] == ':')
			{
				group++;
				inGroup = 0;
				if (group > 7) // max 7 groups when there's "::"
				{
					std::cout << "flag2" << group;
					return (false);
				}
				continue;
			}

			if (value[i] == ']')
				break;
			
			if (!isValidCharIPv6(value[i]))
			{
				std::cout << "flag3";
				return (false);	
			}
			
			inGroup++;
			if (inGroup > 4) // 4 char max between a ':'
			{
				std::cout << "flag4" << inGroup;
				return (false);
			}
		}
	}
	if (value[endIP] != ']') // if the ip isn't terminated by ']'
	{
		std::cout << "flag5" << value[endIP];
		return (false);	
	}
	endIP++;
	if (endIP < value.length() && value[endIP] == ':')
		return (isValidPort(value, endIP + 1));
	if (endIP < value.length())
	{
		std::cout << "flag6" << endIP << " " << value.length();
		return (false);
	}
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
			throw HttpException(400, RED "Headers value contains a non valid character: " YELLOW + value + RESET);
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
	(void)body;
}
