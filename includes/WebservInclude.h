/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservInclude.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 18:01:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/01 19:30:39 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVINCLUDE_H
# define WEBSERVINCLUDE_H

# define RESET "\033[0m"
# define BLACK "\033[1;30m"
# define RED "\033[1;31m"
# define GREEN "\033[1;32m"
# define YELLOW "\033[1;33m"
# define BLUE "\033[1;34m"
# define MAGENTA "\033[1;35m"
# define CYAN "\033[1;36m"
# define WHITE "\033[1;37m"

# include <map>
# include <iostream>
# include <cstring>
# include <stdlib.h>
# include <stdio.h>

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

# include <netinet/in.h>
# include <arpa/inet.h>
# include <cerrno>
# include <poll.h>

# include <unistd.h>
# include <string.h>
# include <vector>

# include <sstream>

struct RequestLine
{
	std::string	_raw_requestLine;
	std::string _method;
	std::string _target;
	std::string _version;
};

struct Header
{
	std::vector<std::pair<std::string, std::string> > _headersFields;
};

struct HttpRequest
{
	RequestLine requestLine;
	Header		header;
	std::string	_body;
};

struct ResponseData
{
	int 		_contentLength;
	std::string _body;
	std::string _contentType;
};

#endif