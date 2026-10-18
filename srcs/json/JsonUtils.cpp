/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonUtils.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/09 10:59:33 by pcaplat           #+#    #+#             */
/*   Updated: 2026/10/09 11:19:37 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/json/JsonToken.hpp"

std::string	strTokenType( TokenType type )
{
	switch (type)
	{
		case TOKEN_LBRACE:
			return std::string("LBRACE");
		case TOKEN_RBRACE:
			return std::string("RBRACE");
		case TOKEN_LBRACKET:
			return std::string("LBRACKET");
		case TOKEN_RBRACKET:
			return std::string("RBRACKET");
		case TOKEN_COLON:
			return std::string("COLON");
		case TOKEN_COMMA:
			return std::string("COMMA");
		case TOKEN_BOOL:
			return std::string("BOOL");
		case TOKEN_STRING:
			return std::string("STRING");
		case TOKEN_NUMBER:
			return std::string("NUMBER");
		case TOKEN_END:
			return std::string("END");
		case TOKEN_NULL:
			return std::string("NULL");
	}
	return std::string();
}
