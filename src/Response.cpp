/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/06 16:59:57 by aleite-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "main.hpp"

Response::Response(): _is_dir(false), _is_autoindex(false)
{
	_status[200] = "OK";
	_status[201] = "Created";
	_status[202] = "Accepted";
	_status[204] = "No Content";
	_status[301] = "Moved Permanently";
	_status[302] = "Found";
	_status[400] = "Bad Request";
	_status[401] = "Unauthorized";
	_status[403] = "Forbidden";
	_status[404] = "Not Found";
	_status[415] = "Unsupported Media Type";
}

Response::~Response()
{

}

void Response::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

void Response::setHeaders(Request &req, int flag, std::string cgiBody, struct RouteConfig route)
{
	checkOpenFile(req.getPath(), req, route);
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
	if (req.getPath().find("cgi-bin") == std::string::npos && _statusCode == 200)
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
	this->_response += version + " " + ss.str() + " " + _status[_statusCode] + "\n";
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

void Response::openListTree()
{
	AutoIndex index;
	std::string str = index.generateAutoIndexHTML(this->_path);
	std::stringstream ss;
	ss << str.length();
	this->_response += "HTTP/1.1 200 OK\nContent-Length: " + ss.str() + "\nContent-Type: text/html\r\n\r\n";
	this->_response += str;
}

void Response::openDirectory(struct RouteConfig route)
{
	this->_response += "HTTP/1.1 301 Moved Permanently\nLocation: ./" + route.default_page + "\nContent-Length: 0\nConnection: close\n\n";
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
		page.open((path).c_str());
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
	
	// path.erase(0, 1);
	if (code == "200")
		page.open((path).c_str());
	else
		page.open(("pages/" + code + ".html").c_str());
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

std::string Response::handleCGI(Request &req)
{
	std::string path = req.getPath();
	_cgi.setCgiPath(path);
	_cgi.setCgiEnv(req);
	return (_cgi.execute(req));
}

std::ostream& operator<<(std::ostream &os, Response const &f)
{
	os << "Response : " << f.getResponse();
	return (os);
}