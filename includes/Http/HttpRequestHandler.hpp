/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.hpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:31 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:37:27 by anfouger         ###   ########.fr       */
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

	// === GET === //
	ResponseData& handleGet(HttpRequest httpRequest);
	// === POST === //
	ResponseData& handlePost(HttpRequest httpRequest);
	// === DELETE === //
	ResponseData& handleDelete(HttpRequest httpRequest);
public:
	HttpRequestHandler();
	~HttpRequestHandler();

	ResponseData& handleRequest(HttpRequest httpRequest);
	
};


#endif