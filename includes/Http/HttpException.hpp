/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpException.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/03 19:03:41 by anfouger          #+#    #+#             */
/*   Updated: 2026/09/03 19:08:29 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HTTPEXCEPTION_HPP
# define HTTPEXCEPTION_HPP

# include <exception>
# include <string>

class HttpException : public std::exception
{
private:
	int         _statusCode;
    std::string _message;
public:
	HttpException(int statusCode, const std::string& message);
	virtual ~HttpException() throw();

	virtual const char * what() const throw();

	int	getStatusCode() const;
};

#endif