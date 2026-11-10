/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 18:11:08 by anfouger          #+#    #+#             */
/*   Updated: 2026/11/10 16:14:05 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPPARSER_HPP
# define HTTPPARSER_HPP
# include "../../includes/WebservInclude.h"
# include "../../includes/Http/HttpException.hpp"
# include "../../includes/Utils/Utils.hpp"

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
	void		VerifyKnownHeaders(HeaderList::iterator& headerFields, std::string& name);
	void		VerifyContentLength(HeaderList::iterator& headerFields, std::string& name);
	void		VerifyConnection(HeaderList::iterator& headerFields);
	void		VerifyHost(HeaderList::iterator& headerFields, std::string& name);
	void		VerifyContentType(HeaderList::iterator& headerFields, std::string& name);
	void		VerifyTransfertEncoding(HeaderList::iterator& headerFields, std::string& name);
	bool		isValidHostname(std::string& value);
	bool		isValidCharHostname(char c);
	bool		isValidIPv4(std::string value);
	bool		isPotentialIPv4(std::string& value);
	bool		isValidIPv6(std::string value);
	bool		isValidCharIPv6(char c);
	bool		isValidPort(std::string& value, int i);
	bool		isValidCharValue(char c);
	bool		isValidToken(const std::string& str);
	bool		isWrongDupplicate(HeaderList::iterator& headerFields, std::string& name);
	bool		isDupplicate(HeaderList::iterator& headerFields, std::string& name);

public:
	HttpParser();
	~HttpParser();

	// === HEADER === //
	HttpRequest	ParseHttpRequest(std::string& header);
};

#endif
