/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:31 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/25 16:53:46 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP

# include <WebservInclude.h>
# include <Utils.hpp>
# include <configs.h>

class HttpRequestHandler
{
private:
	ResponseData	_responseData;
	std::map<std::string, std::string> _typeTable;
	const ServerConfig		*_config;
	const LocationConfig	*_location;

	std::string	getRequestTarget(std::string& target);
	std::string	getExtension(const std::string& target);

	// === INIT === //
	void	initTypeTable(void);
	// === LOCATION === //
	const LocationConfig	*findLocation(const std::string& target) const;
	bool	isMethodAllowed(const std::string& method) const;
	// === FILES === //
	bool	readFile(const std::string& path);
	void	setErrorPage(void);
	ResponseData	errorResponse(int statusCode);
	// === GET === //
	void handleGet(HttpRequest& httpRequest);
	void handleDirectory(const std::string& target, const std::string& path);
	void createListing(const std::string& target, const std::string& path);
	// === POST === //
	void handlePost(HttpRequest& httpRequest);
	std::string	getHeader(const HttpRequest& req, const std::string& name) const;
	// === DELETE === //
	void handleDelete(HttpRequest& httpRequest);
public:
	HttpRequestHandler();
	HttpRequestHandler(const ServerConfig *config);
	~HttpRequestHandler();

	// === MAIN FUNCTION === //
	ResponseData handleRequest(HttpRequest& httpRequest);
	ResponseData handleError(int statusCode);

};
#endif
