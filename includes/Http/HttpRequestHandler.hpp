/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:31 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/14 19:50:11 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPREQUESTHANDLER_HPP
# define HTTPREQUESTHANDLER_HPP

# include <WebservInclude.h>
# include <Utils.hpp>

class HttpRequestHandler
{
private:
	ResponseData	_responseData;
	std::map<std::string, std::string> _typeTable;

	std::string	getRequestTarget(std::string& target);
	std::string	getExtension(const std::string& target);
	
	// === INIT === //
	void	initTypeTable(void);
	// === GET === //
	void handleGet(HttpRequest& httpRequest);
	// === POST === //
	void handlePost(HttpRequest& httpRequest);
	// === DELETE === //
	void handleDelete(HttpRequest& httpRequest);
public:
	HttpRequestHandler();
	~HttpRequestHandler();

	// === MAIN FUNCTION === //
	ResponseData handleRequest(HttpRequest& httpRequest);

};
#endif