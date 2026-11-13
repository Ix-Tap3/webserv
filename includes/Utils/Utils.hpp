/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marudlof <marudlof@student.42angouleme.fr  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/13 17:53:13 by marudlof          #+#    #+#             */
/*   Updated: 2026/11/13 17:53:30 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include "../WebservInclude.h"

namespace Utils
{
	std::vector<std::string>	strSplit(const std::string &s, char delim);
	int			stringToInt(std::string str);
	std::string	strToMin(std::string& str);
	std::string	intToString(int nb);
	bool		isTchar(char c);
	void		DeleteUselessSpace(std::string& str);
}

#endif
