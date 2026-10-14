/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonUnexpectedTokenException.hpp                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/14 12:00:44 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/14 12:02:13 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include "JsonToken.hpp"

class JsonUnexpectedTokenException:	public std::exception
{
private:
	std::string	_msg;

public:
	JsonUnexpectedTokenException	( Token token );
	JsonUnexpectedTokenException	( char value, std::size_t line, std::size_t col );
	~JsonUnexpectedTokenException	( void ) throw();

	const char	*what( void ) const throw();
};
