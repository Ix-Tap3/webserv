/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marudlof < marudlof@student.42angouleme.f  +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/11/10 15:53:04 by marudlof          #+#    #+#             */
/*   Updated: 2026/11/10 16:51:47 by marudlof         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/cgis/CgiHandler.hpp"
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include <cctype>
#include <ctime>

CgiHandler::CgiHandler(): _pid(-1), _inFd(-1), _outFd(-1), _startTime(0), _written(0)
{
}

CgiHandler::~CgiHandler()
{
}

// === GETTERS === //
int	CgiHandler::getInFd() const { return (this->_inFd); }
int	CgiHandler::getOutFd() const { return (this->_outFd); }

// === START === //
bool	CgiHandler::start(const HttpRequest& req, const CgiInfo& info)
{
	int	inPipe[2];
	int	outPipe[2];

	if (pipe(inPipe) == -1)
		return (false);
	if (pipe(outPipe) == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		return (false);
	}

	std::cout << std::flush;
	this->_pid = fork();
	if (this->_pid == -1)
	{
		close(inPipe[0]);
		close(inPipe[1]);
		close(outPipe[0]);
		close(outPipe[1]);
		return (false);
	}
	if (this->_pid == 0)
		execChild(inPipe, outPipe, req, info);

	close(inPipe[0]);
	close(outPipe[1]);
	this->_inFd = inPipe[1];
	this->_outFd = outPipe[0];
	fcntl(this->_inFd, F_SETFL, O_NONBLOCK);
	fcntl(this->_outFd, F_SETFL, O_NONBLOCK);

	this->_body = req._body;
	this->_written = 0;
	this->_output.clear();
	this->_startTime = std::time(NULL);

	if (this->_body.empty())
	{
		close(this->_inFd);
		this->_inFd = -1;
	}
	return (true);
}

// === CHILD === //
void	CgiHandler::execChild(int inPipe[2], int outPipe[2], const HttpRequest& req, const CgiInfo& info) const
{
	std::vector<std::string>	env = buildEnv(req, info);
	std::vector<char *>			envp;
	char						*argv[3];

	for (size_t i = 0; i < env.size(); i++)
		envp.push_back(const_cast<char *>(env[i].c_str()));
	envp.push_back(NULL);


	argv[0] = const_cast<char *>(info._interpreter.c_str());
	argv[1] = const_cast<char *>(info._scriptPath.c_str());
	argv[2] = NULL;

	if (dup2(inPipe[0], STDIN_FILENO) == -1 || dup2(outPipe[1], STDOUT_FILENO) == -1)
		std::exit(1);

	for (int fd = 3; fd < CGI_MAX_FD; fd++)
		close(fd);

	std::string	dir = info._scriptPath.substr(0, info._scriptPath.find_last_of('/'));

	if (chdir(dir.c_str()) == -1)
		std::exit(1);

	execve(argv[0], argv, &envp[0]);
	std::exit(1);
}

std::vector<std::string>	CgiHandler::buildEnv(const HttpRequest& req, const CgiInfo& info) const
{
	std::vector<std::string>	env;

	env.push_back("GATEWAY_INTERFACE=CGI/1.1");
	env.push_back("SERVER_PROTOCOL=HTTP/1.1");
	env.push_back("SERVER_SOFTWARE=webserv");
	env.push_back("REDIRECT_STATUS=200");
	env.push_back("REQUEST_METHOD=" + req.requestLine._method);
	env.push_back("QUERY_STRING=" + info._query);
	env.push_back("SCRIPT_NAME=" + info._scriptName);
	env.push_back("SCRIPT_FILENAME=" + info._scriptPath);
	env.push_back("PATH_INFO=" + info._scriptName);
	env.push_back("CONTENT_LENGTH=" + Utils::intToString(req._body.size()));

	for (HeaderList::const_iterator it =
		req.header._headersFields.begin(); it != req.header._headersFields.end(); ++it)
	{
		std::string	name = it->first;
		std::string	lower = Utils::strToMin(name);

		if (lower == "content-length")
			continue ;
		if (lower == "content-type")
		{
			env.push_back("CONTENT_TYPE=" + it->second);
			continue ;
		}
		if (lower == "host")
		{
			size_t	colon = it->second.find(':');

			env.push_back("SERVER_NAME=" + it->second.substr(0, colon));
			if (colon != std::string::npos)
				env.push_back("SERVER_PORT=" + it->second.substr(colon + 1));
		}

		std::string	key = "HTTP_";

		for (size_t i = 0; i < name.size(); i++)
		{
			if (name[i] == '-')
				key += '_';
			else
				key += static_cast<char>(std::toupper(static_cast<unsigned char>(name[i])));
		}
		env.push_back(key + "=" + it->second);
	}
	return (env);
}

int	CgiHandler::writeBody()
{
	ssize_t	n = 0;

	n += write(this->_inFd, this->_body.c_str() + this->_written, this->_body.size() - this->_written);
	if (n <= 0)
		return (-1);
	this->_written += n;
	if (this->_written >= this->_body.size())
		return (1);
	return (0);
}

int	CgiHandler::readOutput()
{
	char	buf[65536];
	ssize_t	n = read(this->_outFd, buf, sizeof(buf));

	if (n < 0)
		return (-1);
	if (n == 0)
		return (1);
	this->_output.append(buf, n);
	return (0);
}

void	CgiHandler::forgetFd(int fd)
{
	if (fd == this->_inFd)
		this->_inFd = -1;
	if (fd == this->_outFd)
		this->_outFd = -1;
}

// === FIN === //
bool	CgiHandler::isTimedOut() const
{
	return (std::difftime(std::time(NULL), this->_startTime) > CGI_TIMEOUT);
}

void	CgiHandler::stop()
{
	if (this->_pid <= 0)
		return ;
	if (waitpid(this->_pid, NULL, WNOHANG) == 0)
	{
		kill(this->_pid, SIGKILL);
		waitpid(this->_pid, NULL, 0);
	}
	this->_pid = -1;
}

// === REPONSE === //
ResponseData	CgiHandler::buildResponse() const
{
	ResponseData	res;
	size_t			crlf = this->_output.find("\r\n\r\n");
	size_t			lf = this->_output.find("\n\n");
	size_t			sep = crlf;
	size_t			sepLen = 4;

	if (lf != std::string::npos && (crlf == std::string::npos || lf < crlf))
	{
		sep = lf;
		sepLen = 2;
	}
	if (sep == std::string::npos)
	{
		res._statusCode = 502;
		return (res);
	}

	res._statusCode = 200;

	std::istringstream	headers(this->_output.substr(0, sep));
	std::string			line;

	while (std::getline(headers, line))
	{
		if (!line.empty() && line[line.size() - 1] == '\r')
			line.erase(line.size() - 1);
		if (!line.empty())
			parseHeaderLine(line, res);
	}
	if (res._statusCode < 100 || res._statusCode > 599)
	{
		res._statusCode = 502;
		return (res);
	}

	res._body = this->_output.substr(sep + sepLen);
	res._contentLength = res._body.size();
	if (res._contentType.empty())
		res._contentType = "text/html";
	return (res);
}

void	CgiHandler::parseHeaderLine(const std::string& line, ResponseData& res) const
{
	size_t	colon = line.find(':');

	if (colon == std::string::npos)
		return ;

	std::string	name = line.substr(0, colon);
	std::string	value = line.substr(colon + 1);
	std::string	lower = Utils::strToMin(name);

	Utils::DeleteUselessSpace(value);
	if (lower == "status")
		res._statusCode = std::atoi(value.c_str());
	else if (lower == "content-type")
		res._contentType = value;
	else if (lower == "location")
	{
		res._location = value;
		if (res._statusCode == 200)
			res._statusCode = 302;
	}
	else if (lower != "content-length" && lower != "connection")
		res._extraHeaders += name + ": " + value + "\r\n";
}
