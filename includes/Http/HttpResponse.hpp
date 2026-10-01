/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 17:37:51 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:37:14 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <WebservInclude.h>
#include <Utils.hpp>

class HttpResponse
{
private:
	
public:
	HttpResponse();
	~HttpResponse();

	std::string CreateHttpResponse(ResponseData& responseData);
};


#endif