/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/04 20:21:05 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

Response::Response()
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

void Response::setHeaders(Request &req, int flag, std::string cgiBody)
{
	checkOpenFile(req.getPath(), req);
	setVersion(req.getVersion());
	if (req.getPath().find("cgi-bin") == std::string::npos && _statusCode == 200)
		setContentType(req.getContentType());
	else
		setContentType("text/html");
	std::stringstream ss;
	ss << readFile(getStatusCode(), req.getPath()).length();
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
		setBody(getStatusCode(), req.getPath());
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

void Response::checkOpenFile(std::string path, Request &req)
{
	std::ifstream page;
	
	if (path.find("cgi-bin") != std::string::npos)
	{
		path.erase(0, 1);
		page.open((path).c_str());
	}
	else
		page.open(("pages" + path).c_str());
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
	
	if (path.find("pages/cgi-bin") != std::string::npos)
		path.erase(0, 6);
	if (code == "200")
		page.open(("pages" + path).c_str());
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