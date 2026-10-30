/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpRequestHandler.cpp                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: anfouger <anfouger@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/01 18:25:22 by anfouger          #+#    #+#             */
/*   Updated: 2026/10/30 14:25:47 by anfouger         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <HttpRequestHandler.hpp>
#include <string>
#include <sys/stat.h>
#include <dirent.h>

// === INIT === //

HttpRequestHandler::HttpRequestHandler(): _config(NULL), _location(NULL)
{
	initTypeTable();
	this->_responseData._connection = 0;
}

HttpRequestHandler::HttpRequestHandler(const ServerConfig *config): _config(config), _location(NULL)
{
	initTypeTable();
	this->_responseData._connection = 0;
}

HttpRequestHandler::~HttpRequestHandler()
{
}

void	HttpRequestHandler::initTypeTable(void)
{
	this->_typeTable[".html"] = "text/html";
	this->_typeTable[".css"] = "text/css";
	this->_typeTable[".js"] = "application/javascript";
	this->_typeTable[".txt"] = "text/plain";
	this->_typeTable[".json"] = "application/json";
	this->_typeTable[".png"] = "image/png";
	this->_typeTable[".jpg"] = "image/jpeg";
	this->_typeTable[".gif"] = "image/gif";
	this->_typeTable[".svg"] = "image/svg+xml";
	this->_typeTable[".ico"] = "image/x-icon";
}

std::string HttpRequestHandler::getRequestTarget(std::string& target)
{
	size_t		q = target.find('?');
	std::string	query;

	if (q != std::string::npos)
	{
		query = target.substr(q + 1);
		target.erase(q);
	}
	return (query);
}

std::string HttpRequestHandler::getExtension(const std::string& target)
{
	std::string extension;

	size_t pos_extension = target.find_last_of('.');
	size_t pos_slash = target.find_last_of('/');
	if (pos_extension == std::string::npos
		|| (pos_slash != std::string::npos && pos_extension < pos_slash))
		return (extension);
	
	extension = target.substr(pos_extension, target.size() - 1);
	return (extension);
}

// === MAIN FUNCTION === //
ResponseData HttpRequestHandler::handleRequest(HttpRequest& httpRequest)
{
	this->_responseData = ResponseData();
	this->_location = NULL;
	if (this->_config == NULL)
		return (errorResponse(500));

	const std::string&	method = httpRequest.requestLine._method;
	std::string&		target = httpRequest.requestLine._target;

	getRequestTarget(target);
	if (target.find("..") != std::string::npos)
		return (errorResponse(403));

	this->_location = findLocation(target);
	if (this->_location == NULL)
		return (errorResponse(404));

	if (this->_location->redirect_code != -1)
	{
		this->_responseData._statusCode = this->_location->redirect_code;
		this->_responseData._location = this->_location->redirect_path;
		return (this->_responseData);
	}
	if (!isMethodAllowed(method))
		return (errorResponse(405));
	if (httpRequest._body.size() > static_cast<size_t>(this->_location->max_body_size))
		return (errorResponse(413));

	if (method == "GET")
		handleGet(httpRequest);
	else if (method == "POST")
		handlePost(httpRequest);
	else if (method == "DELETE")
		handleDelete(httpRequest);
	else
		this->_responseData._statusCode = 501;

	if (this->_responseData._statusCode >= 400)
		setErrorPage();

	handleCookie(httpRequest);

	return (this->_responseData);
}

ResponseData HttpRequestHandler::handleError(int statusCode)
{
	this->_responseData = ResponseData();
	this->_location = NULL;
	return (errorResponse(statusCode));
}

// === LOCATION === //
int	HttpRequestHandler::getMaxBodySize(const std::string& target) const
{
	if (this->_config == NULL)
		return (-1);
	const LocationConfig	*location = findLocation(target.substr(0, target.find('?')));
	if (location == NULL)
		return (this->_config->max_body_size);
	return (location->max_body_size);
}

const LocationConfig	*HttpRequestHandler::findLocation(const std::string& target) const
{
	const LocationConfig	*best = NULL;

	for (std::vector<LocationConfig>::const_iterator it = this->_config->locations.begin();
		it != this->_config->locations.end(); ++it)
	{
		const std::string&	path = it->path;

		if (target.compare(0, path.size(), path) != 0)
			continue ;
		if (target.size() != path.size() && path[path.size() - 1] != '/'
			&& target[path.size()] != '/')
			continue ;
		if (best == NULL || path.size() > best->path.size())
			best = &(*it);
	}
	return (best);
}

bool	HttpRequestHandler::isMethodAllowed(const std::string& method) const
{
	for (std::vector<std::string>::const_iterator it = this->_location->methods.begin();
		it != this->_location->methods.end(); ++it)
	{
		if (*it == method)
			return (true);
	}
	return (false);
}

// === FILES === //
bool	HttpRequestHandler::readFile(const std::string& path)
{
	std::ifstream file(path.c_str(), std::ios::binary);
	if (!file.is_open())
		return (false);

	this->_responseData._body.clear();
	char buf[1000];
	file.read(buf, sizeof(buf));
	std::streamsize n = file.gcount();
	while (n > 0)
	{
		this->_responseData._body.append(buf, n);
		file.read(buf, sizeof(buf));
		n = file.gcount();
	}
	this->_responseData._contentLength = this->_responseData._body.size();

	std::map<std::string, std::string>::iterator it = this->_typeTable.find(getExtension(path));
	if (it != this->_typeTable.end())
		this->_responseData._contentType = it->second;
	return (true);
}

void	HttpRequestHandler::setErrorPage(void)
{
	this->_responseData._body.clear();
	this->_responseData._contentLength = 0;
	this->_responseData._contentType.clear();
	if (this->_config == NULL)
		return ;

	const std::map<int, std::string>	*pages = &this->_config->error_pages;
	const std::string					*root = &this->_config->root;
	if (this->_location)
	{
		pages = &this->_location->error_pages;
		root = &this->_location->root;
	}
	std::map<int, std::string>::const_iterator it = pages->find(this->_responseData._statusCode);

	if (it == pages->end() || !readFile(*root + it->second))
	{
		this->_responseData._body.clear();
		this->_responseData._contentLength = 0;
	}
}

ResponseData	HttpRequestHandler::errorResponse(int statusCode)
{
	this->_responseData._statusCode = statusCode;
	setErrorPage();
	return (this->_responseData);
}

// === GET === //
void HttpRequestHandler::handleGet(HttpRequest& httpRequest)
{
	const std::string&	target = httpRequest.requestLine._target;
	std::string			path = this->_location->root + target;
	struct stat			st;

	if (access(path.c_str(), F_OK) == -1)
	{
		this->_responseData._statusCode = 404;
		return ;
	}
	else if (access(path.c_str(), R_OK) == -1 || stat(path.c_str(), &st) == -1)
	{
		this->_responseData._statusCode = 403;
		return ;
	}
	if (S_ISDIR(st.st_mode))
	{
		handleDirectory(target, path);
		return ;
	}
	if (!readFile(path))
	{
		this->_responseData._statusCode = 403;
		return ;
	}
	this->_responseData._statusCode = 200;
}

void HttpRequestHandler::handleDirectory(const std::string& target, const std::string& path)
{
	if (target[target.size() - 1] != '/')
	{
		this->_responseData._statusCode = 301;
		this->_responseData._location = target + "/";
		return ;
	}

	std::string	index = path.substr(0, path.size() - 1) + this->_location->default_page;
	if (access(index.c_str(), R_OK) == 0 && readFile(index))
	{
		this->_responseData._statusCode = 200;
		return ;
	}
	if (this->_location->directory_listing)
	{
		createListing(target, path);
		return ;
	}
	this->_responseData._statusCode = 403;
}

static std::string	htmlEscape(const std::string& str)
{
	std::string	res;

	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '&')
			res += "&amp;";
		else if (str[i] == '<')
			res += "&lt;";
		else if (str[i] == '>')
			res += "&gt;";
		else if (str[i] == '"')
			res += "&quot;";
		else
			res += str[i];
	}
	return (res);
}

void HttpRequestHandler::createListing(const std::string& target, const std::string& path)
{
	DIR	*dir = opendir(path.c_str());
	if (dir == NULL)
	{
		this->_responseData._statusCode = 403;
		return ;
	}

	std::string&	body = this->_responseData._body;
	std::string		title = "Index of " + htmlEscape(target);

	body = "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>" + title + "</title></head>\n";
	body += "<body>\n<h1>" + title + "</h1>\n<ul>\n";
	for (struct dirent *entry = readdir(dir); entry != NULL; entry = readdir(dir))
	{
		std::string	name = entry->d_name;

		if (name == ".")
			continue ;
		if (entry->d_type == DT_DIR)
			name += "/";
		body += "<li><a href=\"" + htmlEscape(name) + "\">" + htmlEscape(name) + "</a></li>\n";
	}
	closedir(dir);
	body += "</ul>\n</body>\n</html>\n";

	this->_responseData._contentLength = body.size();
	this->_responseData._contentType = "text/html";
	this->_responseData._statusCode = 200;
}

// === POST === //
void HttpRequestHandler::handlePost(HttpRequest& httpRequest)
{
	if (this->_location->upload.empty())
	{
		this->_responseData._statusCode = 403; // plus tard : CGI
		return ;
	}
	if (!isUploadDirUsable(this->_location->upload))
	{
		this->_responseData._statusCode = 500;
		return ;
	}
	std::string contentType = getHeader(httpRequest, "content-type");
	if (!isMultipart(contentType))
	{
		handleRawPost(httpRequest);
		return ;
	}
	handleMultipart(httpRequest._body, contentType);
}

bool	HttpRequestHandler::isUploadDirUsable(const std::string& dir) const
{
	struct stat	st;

	if (stat(dir.c_str(), &st) != 0)
		return (false);
	if (!S_ISDIR(st.st_mode))
		return (false);
	return (access(dir.c_str(), W_OK | X_OK) == 0);
}

bool	HttpRequestHandler::isMultipart(const std::string& contentType) const
{
	std::string	type = contentType;

	type = Utils::strToMin(type);
	return (type.compare(0, 19, "multipart/form-data") == 0);
}

std::string	HttpRequestHandler::extractBoundary(const std::string& contentType) const
{
	std::string	lower = contentType;

	lower = Utils::strToMin(lower);
	size_t	start = lower.find("boundary=");
	if (start == std::string::npos)
		return ("");
	start += 9;

	std::string	boundary;
	if (start < contentType.size() && contentType[start] == '"')
	{
		size_t	end = contentType.find('"', start + 1);
		if (end == std::string::npos)
			return ("");
		boundary = contentType.substr(start + 1, end - start - 1);
	}
	else
	{
		size_t	end = contentType.find(';', start);
		if (end == std::string::npos)
			end = contentType.size();
		boundary = contentType.substr(start, end - start);
	}
	Utils::DeleteUselessSpace(boundary);
	return (boundary);
}

bool	HttpRequestHandler::parseMultipart(const std::string& body, const std::string& delim,
			std::vector<MultipartPart>& parts) const
{
	size_t	pos = body.find(delim);
	if (pos == std::string::npos)
		return (false);
	while (42)
	{
		pos += delim.size();
		if (body.compare(pos, 2, "--") == 0)
			return (true);
		if (body.compare(pos, 2, "\r\n") != 0)
			return (false);
		pos += 2;

		MultipartPart	part;
		size_t			dataStart;
		if (body.compare(pos, 2, "\r\n") == 0)
			dataStart = pos + 2;
		else
		{
			size_t	headersEnd = body.find("\r\n\r\n", pos);
			if (headersEnd == std::string::npos)
				return (false);
			part.headers = body.substr(pos, headersEnd - pos);
			dataStart = headersEnd + 4;
		}
		size_t	next = body.find("\r\n" + delim, dataStart);
		if (next == std::string::npos)
			return (false);
		part.data = body.substr(dataStart, next - dataStart);
		parts.push_back(part);
		pos = next + 2;
	}
}

std::string	HttpRequestHandler::extractFilename(const std::string& partHeaders) const
{
	std::string	lower = partHeaders;

	lower = Utils::strToMin(lower);
	size_t	start = lower.find("filename=\"");
	if (start == std::string::npos)
		return ("");
	start += 10;
	size_t	end = partHeaders.find('"', start);
	if (end == std::string::npos)
		return ("");
	return (partHeaders.substr(start, end - start));
}

std::string	HttpRequestHandler::sanitizeFilename(const std::string& raw) const
{
	std::string	name = raw;

	size_t	slash = name.find_last_of("/\\");
	if (slash != std::string::npos)
		name = name.substr(slash + 1);
	if (name.empty() || name == "." || name == "..")
		return ("");
	for (size_t i = 0; i < name.size(); i++)
	{
		unsigned char	c = static_cast<unsigned char>(name[i]);
		if (c < ' ' || c > '~')
			return ("");
	}
	return (name);
}

bool	HttpRequestHandler::writeFile(const std::string& path, const std::string& data) const
{
	std::ofstream	out(path.c_str(), std::ios::binary | std::ios::trunc);
	if (!out.is_open())
		return (false);
	out.write(data.data(), data.size());
	if (!out)
	{
		out.close();
		std::remove(path.c_str());
		return (false);
	}
	return (true);
}

int	HttpRequestHandler::handlePart(const MultipartPart& part, std::string& savedName)
{
	std::string	raw = extractFilename(part.headers);
	if (raw.empty())
		return (0);
	std::string	filename = sanitizeFilename(raw);
	if (filename.empty())
		return (400);
	std::string	path = this->_location->upload + "/" + filename;
	if (access(path.c_str(), F_OK) == 0)
		return (409);
	if (!writeFile(path, part.data))
		return (500);
	savedName = filename;
	return (201);
}

void	HttpRequestHandler::setUploadSuccess(const std::string& filename)
{
	std::string	url = this->_location->path;
	if (url.empty() || url[url.size() - 1] != '/')
		url += "/";
	url += filename;

	std::string&	body = this->_responseData._body;
	body = "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Upload</title></head>\n";
	body += "<body>\n<h1>Fichier uploadé</h1>\n";
	body += "<p><a href=\"" + htmlEscape(url) + "\">" + htmlEscape(filename) + "</a></p>\n";
	body += "<p><a href=\"" + htmlEscape(this->_location->path) + "\">Retour</a></p>\n";
	body += "</body>\n</html>\n";

	this->_responseData._location = url;
	this->_responseData._contentLength = body.size();
	this->_responseData._contentType = "text/html";
	this->_responseData._statusCode = 201;
}

void	HttpRequestHandler::handleRawPost(HttpRequest& httpRequest)
{
	const std::string&	target = httpRequest.requestLine._target;

	std::string	filename = sanitizeFilename(target.substr(this->_location->path.size()));
	if (filename.empty())
	{
		this->_responseData._statusCode = 400;
		return ;
	}
	std::string	path = this->_location->upload + "/" + filename;
	if (access(path.c_str(), F_OK) == 0)
	{
		this->_responseData._statusCode = 409;
		return ;
	}
	if (!writeFile(path, httpRequest._body))
	{
		this->_responseData._statusCode = 500;
		return ;
	}
	setUploadSuccess(filename);
}

void	HttpRequestHandler::handleMultipart(const std::string& body, const std::string& contentType)
{
	std::string	boundary = extractBoundary(contentType);
	if (boundary.empty())
	{
		this->_responseData._statusCode = 400;
		return ;
	}
	std::vector<MultipartPart>	parts;
	if (!parseMultipart(body, "--" + boundary, parts))
	{
		this->_responseData._statusCode = 400;
		return ;
	}
	std::string	savedName;
	for (size_t i = 0; i < parts.size(); i++)
	{
		int	code = handlePart(parts[i], savedName);
		if (code >= 400)
		{
			this->_responseData._statusCode = code;
			return ;
		}
	}
	if (savedName.empty())
	{
		this->_responseData._statusCode = 400;
		return ;
	}
	setUploadSuccess(savedName);
}

std::string	HttpRequestHandler::getHeader(const HttpRequest& req, const std::string& name) const
{
	for (std::vector<std::pair<std::string, std::string> >::const_iterator it =
		req.header._headersFields.begin();
		it != req.header._headersFields.end();
		++it)
	{
		std::string fieldName = it->first;
		if (Utils::strToMin(fieldName) == name)
			return (it->second);
	}
	return ("");
}

// === DELETE === //
void HttpRequestHandler::handleDelete(HttpRequest& httpRequest)
{
	std::string	path = this->_location->root + httpRequest.requestLine._target;
	struct stat	st;

	if (stat(path.c_str(), &st) == -1)
		this->_responseData._statusCode = 404;
	else if (S_ISDIR(st.st_mode))
		this->_responseData._statusCode = 403;
	else if (std::remove(path.c_str()) != 0)
	{
		if (errno == EACCES)
			this->_responseData._statusCode = 403;
		else
			this->_responseData._statusCode = 500;
	}
	else
	{
		this->_responseData._statusCode = 204;
		this->_responseData._body.clear();
		this->_responseData._contentLength = 0;
	}
}
void	HttpRequestHandler::handleCookie(HttpRequest& httpRequest)
{
	std::srand(std::time(NULL)); // set the seed for the randomisation
	initLanguageTable();
	initAttributsVector();

	for (std::vector<std::pair<std::string, std::string> >::iterator it =
	httpRequest.header._headersFields.begin(); 
	it != httpRequest.header._headersFields.end(); 
	++it)
	{
		std::string name = Utils::strToMin(it->first);
		if (name == "cookie") // there's a cookie header
		{
			std::string sessionID = getSessionID(it->second);
			// search for session ID
			std::map<std::string, std::map<std::string, std::string> >::iterator session = this->_cookiesTable.find(sessionID);
			if (session == this->_cookiesTable.end())
				break;
			
			// add new cookies if there is

			// assemble cookie
			this->_responseData._cookie = assembleCookie(sessionID);
			return ;
		}
	} 
	// there's no cookie header
	// create new session ID and new cookies
	std::string sessionID = generateSessionID();
	this->_cookiesTable[sessionID]["lang"] = getRandomLanguage();
	this->_cookiesTable[sessionID]["theme"] = "dark";

	// assemble cookie
	this->_responseData._cookie = assembleCookie(sessionID);
}

void	HttpRequestHandler::initLanguageTable(void)
{
	static const char	*languages[] = {
		"en", "fr", "de", "es", "it", "pt", "nl", "sv", "da", "no",
		"fi", "pl", "cs", "sk", "hu", "ro", "bg", "el", "tr", "ru",
		"uk", "he", "ar", "fa", "hi", "bn", "th", "vi", "id", "ms",
		"zh", "ja", "ko", "ca", "hr", "sr", "sl", "lt", "lv", "et"
	};

	this->_languageTable.assign(languages, languages + sizeof(languages) / sizeof(languages[0]));
}

void	HttpRequestHandler::initAttributsVector(void)
{
	static const char	*attributs[] = {"Path=/", "HttpOnly"};

	this->_attributsTable.assign(attributs, attributs + sizeof(attributs) / sizeof(attributs[0]));

}

std::string	HttpRequestHandler::generateSessionID(void)
{
	std::string sessionID = "";

	std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_cookiesTable.find(sessionID);
	while (this->_cookiesTable.end() == it)
	{
		for (size_t i = 0; i < 32; i++)
		{
			char r = 33 + std::rand() % (122 - 33 + 1);
			sessionID += r;
		}
		std::map<std::string, std::map<std::string, std::string> >::iterator it = this->_cookiesTable.find(sessionID);
	}
	
	return (sessionID);
}

std::string	HttpRequestHandler::getRandomLanguage(void)
{
	return (this->_languageTable[std::rand() % this->_languageTable.size()]);
}

std::string	HttpRequestHandler::assembleCookie(std::string& sessionID)
{
	std::string cookie = "Set-Cookie: sessionID=";

	cookie+= sessionID;
	cookie+= ";";
	for (std::map<std::string, std::string>::iterator it = 
		this->_cookiesTable[sessionID].begin();
		it != this->_cookiesTable[sessionID].end()
		; it++)
	{
		cookie += "\r\n";
		cookie += "Set-Cookie: ";
		cookie += it->first;
		cookie += "=";
		cookie += it->second;
		cookie += "; ";
		for (std::vector<std::string>::iterator it = this->_attributsTable.begin();
			it != this->_attributsTable.end(); it++)
		{
			cookie += *(it);
			cookie += " ";
		}
	}
	return (cookie);
}

std::string	HttpRequestHandler::getSessionID(std::string& cookie)
{
	size_t start = cookie.find("sessionID=") + 10;
	size_t end = cookie.find(";", start);

	return (cookie.substr(start, end));
}
