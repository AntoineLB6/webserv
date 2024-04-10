/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/10 13:50:28 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "main.hpp"

Response::Response(): _is_dir(false), _is_autoindex(false)
{
	setErrorsPages();
}


Response::Response(struct WebConfig config): _is_dir(false), _is_autoindex(false)
{
	setErrorsPages();
	for (std::map<int, std::string>::iterator it = config.errors_pages.begin(); it != config.errors_pages.end(); ++it)
	{
		this->_errors_pages[it->first] = it->second;
	}
}

Response::~Response()
{

}

void Response::setErrorsPages(void)
{
	_errors_pages[200] = "OK";
	_errors_pages[201] = "Created";
	_errors_pages[202] = "Accepted";
	_errors_pages[204] = "No Content";
	_errors_pages[301] = "Moved Permanently";
	_errors_pages[302] = "Found";
	_errors_pages[400] = "Bad Request";
	_errors_pages[401] = "Unauthorized";
	_errors_pages[403] = "Forbidden";
	_errors_pages[404] = "Not Found";
	_errors_pages[413] = "Payload Too Large";
	_errors_pages[415] = "Unsupported Media Type";
}

void Response::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

void Response::setStatus(struct RouteConfig route)
{
	for (std::map<int, std::string>::iterator it = route.return_codes.begin(); it != route.return_codes.end(); it++)
	{
		this->_errors_pages[it->first] = it->second;
	}
}

void Response::setHeaders(Request &req, int flag, std::string cgiBody, struct RouteConfig route)
{
	this->_root = route.root;
	this->setStatus(route);
	checkOpenFile(req.getPath(), req, route);
	if (req.getMethod() == "DELETE")
	{
		this->getDeleteRes(flag);
		return ;
	}
	if (this->_is_autoindex)
	{
		this->openListTree();
		return ;
	}
	if (this->_is_dir)
	{
		this->openDirectory(route);
		return ;
	}
	setVersion(req.getVersion());
	int temp = req.getContentType().substr(0, req.getContentType().find(";")).compare("multipart/form-data");
	if (req.getPath().find("cgi-bin") == std::string::npos && _statusCode == 200 && temp)
	{
		setContentType(req.getContentType());
	}
	else
		setContentType("text/html");
	std::stringstream ss;
	ss << readFile(getStatusCode(), _path).length();
	if (!flag)
		setContentLength(ss.str());
	else
	{
		std::stringstream ss;
		ss << CGIBodyLength(cgiBody);
		setContentLength(ss.str());
	}
	setDate();
	setServer("webserv");
	setConnection("Keep-Alive", req);
	if (req.getPath().find("cgi-bin") == std::string::npos)
		setBody(getStatusCode(), _path);
}

void Response::setVersion(std::string version)
{
	std::stringstream ss;
	ss << _statusCode;
	this->_response += version + " " + ss.str() + " " + _errors_pages[_statusCode] + "\n";
}

void Response::setContentType(std::string contentType)
{
	this->_response += "Content-Type: " + contentType + "\n";
}

void Response::setContentLength(std::string contentLength)
{
	this->_response += "Content-Length: " + contentLength + "\n";
}

void Response::setDate(void)
{
	time_t now = time(0);
	struct tm tstruct;
	char buffer[80];

	tstruct = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%a, %d %b %y, %H:%M:%S %Z", &tstruct);
	std::stringstream ss;
	ss << buffer;
	_response += "Date: " + ss.str() + "\n";
}

void Response::setServer(std::string serverName)
{
	this->_response += "Server: " + serverName + "\n";
}

void Response::setConnection(std::string connection, Request &req)
{
	if (req.getPath().find("cgi-bin") == std::string::npos)
		this->_response += "Connection: " + connection + "\r\n\r\n";
	else
		this->_response += "Connection: " + connection + "\n";
}

int Response::CGIBodyLength(std::string cgiBody)
{
	int i = 0;
	while (cgiBody[i] != '\r')
		i++;
	if (cgiBody.find("\r") != std::string::npos)
        return (cgiBody.substr(i + 4, cgiBody.length() - 1).length());
    else
        return (cgiBody.length());
}

void Response::setBody(std::string code, std::string path)
{
	std::string body = readFile(code, path);
	_response += body;
}

std::string Response::getResponse(void) const
{
	return (_response);
}

std::string Response::getStatusCode(void) const
{
	std::stringstream ss;
	
	ss << _statusCode;
	return (ss.str());
}

std::map<int, std::string> Response::getErrorsPages(void) const
{
	return (_errors_pages);
}

void Response::openListTree()
{
	AutoIndex index;
	std::string str = index.generateAutoIndexHTML(this->_path);
	std::stringstream ss;
	ss << str.length();
	this->_response += "HTTP/1.1 200 " + this->_errors_pages.find(200)->second + "\nContent-Length: " + ss.str() + "\nContent-Type: text/html\r\n\r\n";
	this->_response += str;
}

void Response::openDirectory(struct RouteConfig route)
{
	this->_response += "HTTP/1.1 301 " + this->_errors_pages.find(301)->second + "\nLocation: ./" + route.default_page + "\nContent-Length: 0\nConnection: close\n\n";
}

void Response::getDeleteRes(int flag)
{
	std::stringstream ss;
	ss << flag;
	std::string loc;
	std::string msg;
	switch (flag)
	{
	case 200:
	{
		// loc = "success-delete.html";
		msg = "La ressource a été supprimée avec succès.";
		break;
	}
	case 403:
	{
		// loc = "forbidden-delete.html";
		msg = "Vous n'êtes pas autorisé à supprimer cette ressource.";
		break;
	}
	case 404:
	{
		// loc = "nf-delete.html";
		msg = "La ressource demandée n'a pas été trouvée sur le serveur.";
		break;
	}
	default:
		break;
	}
	// this->_response += "HTTP/1.1 " + ss.str() + " " + this->_errors_pages.find(flag)->second + "\nLocation: pages/" + loc + "\nContent-Type: text/plain\nConnection: close\n\n" + msg;
	this->_response += "HTTP/1.1 " + ss.str() + " " + this->_errors_pages.find(flag)->second + "\nContent-Type: text/plain\nConnection: close\n\n" + msg + "\n\n";
}

int isDirectoryOrIndex(const std::string& path)
{
    struct stat info;
    if (stat(path.c_str(), &info) != 0) {
		if (!path.empty() && path[path.size() - 1] == '/')
		{
        	return -1;
    	}
        return 0;
    }
    return S_ISDIR(info.st_mode);
}

void Response::checkOpenFile(std::string path, Request &req, struct RouteConfig route)
{
	std::ifstream page;
	
	path = route.root + path;
	this->_path = path;
	if (isDirectoryOrIndex(this->_path) == -1)
	{
		this->_is_dir = true;
		return ;
	}
	else if (isDirectoryOrIndex(this->_path) > 0)
	{
		if (route.autoindex)
			this->_is_autoindex = true;
		else
			this->_is_dir = true;
		return ;
	}
	if (path.find("pages/cgi-bin") != std::string::npos)
	{
		path.erase(0, path.find("pages/cgi-bin"));
		page.open((path).c_str());
		this->_path = path;
	}
	else
	{
		page.open((path).c_str());
	}
	if (path.find(".") == std::string::npos && req.getMethod() == "GET")
	{
		setStatusCode(415);
		return ;
	}
	else if (page.good())
	{
		setStatusCode(200);
		page.close();
	}
	else
		setStatusCode(404);
}

std::string Response::readFile(std::string code, std::string path)
{
	std::ifstream page;
	
	if (code == "200")
		page.open((path).c_str());
	else
	{
		if (this->_errors_pages.find(std::atoi(code.c_str())) != this->_errors_pages.end())
		{
			page.open((this->_errors_pages.find(std::atoi(code.c_str()))->second).c_str());
			if (!page.is_open())
			{
				page.close();
				page.open(("pages/" + code + ".html").c_str());
			}
		}
		else
			page.open(("pages/" + code + ".html").c_str());
		
	}
	std::string body;
	std::string line;
	if (page.is_open())
	{
		while (getline(page, line))
			body += line;
	}
	page.close();
	return (body);
}

std::string Response::handleCGI(Request &req, struct RouteConfig route)
{
	std::string path = route.root + req.getPath();
	_cgi.setCgiEnv(req, path);
	return (_cgi.execute(req));
}

std::ostream& operator<<(std::ostream &os, Response const &f)
{
	os << "Response : " << f.getResponse();
	return (os);
}