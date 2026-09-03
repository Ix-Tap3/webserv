/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 19:03:29 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 19:08:52 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpException.hpp>

HttpException::HttpException(int statusCode, const std::string& message) : _message(message), _statusCode(statusCode)
{
}

HttpException::~HttpException()
{
}

const char* HttpException::what() const throw()
{
    return (_message.c_str());
}

int	HttpException::getStatusCode() const
{
	return this->_statusCode;
}
