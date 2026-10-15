/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 17:37:51 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/15 18:08:52 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPRESPONSE_HPP
# define HTTPRESPONSE_HPP

#include <WebservInclude.h>
#include <Utils.hpp>

class HttpResponse
{
private:
	std::string	_response;
	std::map<int, std::string> _reasonPhraseTable;

	// INIT //
	void		initReasonPhraseTable(void);

	// STATUS LINE //
	std::string	createStatusLine(const ResponseData& responseData);
	
public:
	HttpResponse();
	~HttpResponse();

	std::string CreateHttpResponse(const ResponseData& responseData);
};


#endif