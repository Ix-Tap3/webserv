/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:11:08 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 18:44:35 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP
# include <WebservInclude.h>
# include <HttpException.hpp>
# include <Utils.hpp>

class HttpParser
{
private:
	HttpRequest	_httpRequest;

	// === HEADER === //
	void		DataSorting(std::string& header);
	// Request Line //
	RequestLine ParseRequestLine(std::string& strRequestLine);
	void		VerifyRequestLine(RequestLine requestLine);
	void		VerifyMethod(std::string method);
	void		VerifyTarget(std::string target);
	void		VerifyVersion(std::string version);
	// Headers Fields//
	void		ParseHeaders(void);
	void		VerifyHeaderName(std::string name);
	void		VerifyHeaderValue(std::string value);
	void		VerifyKnownHeaders(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	void		VerifyContentLength(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	void		VerifyConnection(std::vector<std::pair<std::string, std::string> >::iterator& headerFields);
	void		VerifyHost(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	void		VerifyContentType(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	bool		isValidHostname(std::string& value);
	bool		isValidCharHostname(char c);
	bool		isValidIPv4(std::string value);
	bool		isPotentialIPv4(std::string& value);
	bool		isValidIPv6(std::string value);
	bool		isValidCharIPv6(char c);
	bool		isValidPort(std::string& value, int i);
	bool		isValidCharValue(char c);
	bool		isValidToken(const std::string& str);
	bool		isWrongDupplicate(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);
	bool		isDupplicate(std::vector<std::pair<std::string, std::string> >::iterator& headerFields, std::string& name);

public:
	HttpParser();
	~HttpParser();

	// === HEADER === //
	HttpRequest	ParseHttpRequest(std::string& header);
};

#endif