/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonToken.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat <pcaplat@42angouleme.fr>           +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/26 17:01:04 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/26 17:05:11 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
# include <string>
# include <cstdlib>

enum TokenType
{
	TOKEN_LBRACE,
	TOKEN_RBRACE,
	TOKEN_LBRACKET,
	TOKEN_RBRACKET,
	TOKEN_COLON,
	TOKEN_COMMA,
	TOKEN_BOOL,
	TOKEN_NUMBER,
	TOKEN_STRING,
	TOKEN_NULL,
	TOKEN_END
};

struct Token
{
	TokenType	type;
	std::string	value;
	std::size_t	pos;
};
