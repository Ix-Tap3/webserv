/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservInclude.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/08/29 18:01:12 by anfouger          #+#    #+#             */
/*   Updated: 2026/11/13 17:46:52 by marudlof         ###   ########.fr       */
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
# define MAX_TIMEOUT 5 // in second for std::difftime()
# define CGI_TIMEOUT 10 // in second, max execution time of a CGI script
# define CGI_MAX_FD 1024 // fds >= 3 and < CGI_MAX_FD are closed in the CGI child

# include <map>
# include <iostream>
# include <cstring>
# include <cstdlib>
# include <cstdio>
# include <ctime>

# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>

# include <netinet/in.h>
# include <arpa/inet.h>
# include <cerrno>
# include <climits>
# include <poll.h>

# include <unistd.h>
# include <vector>

# include <sstream>
# include <fstream>

struct RequestLine
{
	std::string	_raw_requestLine;
	std::string _method;
	std::string _target;
	std::string _version;
};

typedef std::vector<std::pair<std::string, std::string> >	HeaderList;

struct Header
{
	HeaderList	_headersFields;
};

struct HttpRequest
{
	RequestLine requestLine;
	Header		header;
	std::string	_body;
};

struct CgiInfo
{
	bool		_active;		// true si la requete doit etre executee par un CGI
	std::string	_interpreter;	// ex: /bin/bash
	std::string	_scriptPath;	// chemin absolu du script
	std::string	_scriptName;	// chemin du script dans l'URL (ex: /cgi-bin/hello.sh)
	std::string	_query;			// ce qu'il y a apres le '?' dans l'URL

	CgiInfo(): _active(false) {}
};

struct ResponseData
{
	int			_statusCode;
	int 		_contentLength;
	int			_connection;
	std::string	_body;
	std::string	_contentType;
	std::string	_location;
	std::string _cookie;
	std::string	_extraHeaders;	// headers supplementaires deja formates "Nom: valeur\r\n"
	CgiInfo		_cgi;
	ResponseData(): _statusCode(0), _contentLength(0), _connection(0) {}
};

#endif
