/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   JsonType.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pcaplat </var/spool/mail/pcaplat>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 11:40:42 by pcaplat           #+#    #+#             */
/*   Updated: 2026/08/29 11:41:49 by pcaplat          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

enum JsonType
{
	JSON_STRING,
	JSON_NUMBER,
	JSON_OBJECT,
	JSON_ARRAY,
	JSON_BOOL,
	JSON_NULL
};
