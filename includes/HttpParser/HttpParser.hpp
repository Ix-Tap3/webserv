/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:11:08 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 17:37:50 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP
# include <WebservInclude.h>

struct RequestLine
{
	// std::string	str_requestLine;
	std::string method;
	std::string target;
	std::string version;
};

struct Body
{
	// std::string str_body;
	// int			contentLength;
};

struct Header
{
	// std::string str_header;
	std::vector<std::pair<std::string, std::string> > _HeadersFields;
};

struct HttpRequest
{
	RequestLine _RequestLine;
	Header	_Header;
	Body	_Body;
};

class HttpParser
{
private:
	HttpRequest	_HttpRequest;

	// === HEADER === //
	void		DataSorting(std::string& header);
	// Request Line //
	RequestLine ParseRequestLine(std::string& strRequestLine);
	void		VerifyRequestLine(RequestLine requestLine);
	void		VerifyMethod(std::string method);
	void		VerifyTarget(std::string target);
	void		VerifyVersion(std::string version);
	bool		ContainsDotDotSegment(const std::string& path);
public:
	HttpParser();
	~HttpParser();

	// === HEADER === //
	Header	ParseHeader(std::string& header);

	// === BODY === //
	Body	ParseBody(std::string&	body);
};

#endif