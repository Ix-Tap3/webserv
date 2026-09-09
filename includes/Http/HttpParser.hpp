/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:11:08 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/08 18:58:36 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP
# include <WebservInclude.h>
# include <HttpException.hpp>

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

	// === UTILS === //
	std::string	strToMin(std::string& str);
	bool		isTchar(char c);

	// === HEADER === //
	void		DataSorting(std::string& header);
	// Request Line //
	RequestLine ParseRequestLine(std::string& strRequestLine);
	void		VerifyRequestLine(RequestLine requestLine);
	void		VerifyMethod(std::string method);
	void		VerifyTarget(std::string target);
	void		VerifyVersion(std::string version);
	// bool		ContainsDotDotSegment(const std::string& path);
	// Headers //
	void		ParseHeaders(void);
	void		VerifyHeaderName(std::string name);
	void		VerifyHeaderValue(std::string value);
	void		VerifyKnownHeaders(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	bool		isValidCharValue(char c);
public:
	HttpParser();
	~HttpParser();

	// === HEADER === //
	Header	ParseHeader(std::string& header);

	// === BODY === //
	Body	ParseBody(std::string&	body);
};

#endif