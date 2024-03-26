/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/26 21:25:31 by lmoheyma         ###   ########.fr       */
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
}

Response::~Response()
{

}

void Response::setStatusCode(int statusCode)
{
	this->_statusCode = statusCode;
}

std::string Response::getMethod(void) const 
{
	return (_method);
}

std::string Response::getPath(void) const 
{
	return (_path);
}

std::string Response::getVersion(void) const 
{
	return (_version);
}

std::string Response::getResponse(void) const
{
	return (_response);
}

void Response::parseMethod(std::string request)
{
	this->i = request.find_first_of(' ');
	_method = request.substr(0, this->i);
}

void Response::parsePath(std::string request)
{
	size_t j;
	
	j = request.find_first_of(' ', this->i + 1);
	_path = request.substr(i + 1, j - i - 1);
	this->i = j;
}

void Response::parseVersion(std::string request)
{
	size_t	j;

	j = request.find_first_of('\r', this->i + 1);
	_version = request.substr(this->i + 1, j - i - 1);
}

void Response::parseAll(std::string request)
{
	parseMethod(request);
	parsePath(request);
	parseVersion(request);
}

void Response::checkOpenFile(void)
{
	std::ifstream page(("pages" + _path).c_str());
	std::cout << "pages" + _path << std::endl;
	if (page.good())
		setStatusCode(200);
	else
		setStatusCode(404);
}

void Response::setContentType(void)
{
	size_t j;
	std::string extension;

	j = getPath().find_last_of('.');
	if (j != std::string::npos)
	{
		size_t i = getPath().find_first_of(' ', j);
		extension = getPath().substr(j, i - j);
	}
	else
		extension = "";
	if (extension == ".html")
		_contentType = "text/html";
	else if (extension == ".jpg")
		_contentType = "image/jpeg";
	else if (extension == ".png")
		_contentType = "image/png";
	else if (extension == ".pdf")
		_contentType = "application/pdf";
	else if (extension == ".php")
		_contentType = "application/x-httpd-php";
	else if (extension == ".js")
		_contentType = "text/javascript";
	else if (extension == ".css")
		_contentType = "text/css";
	else if (extension == ".bmp")
		_contentType = "image/bmp";
	else if (extension == ".txt")
		_contentType = "text/plain";
	else
		_contentType = "text/plain";
}

std::string Response::readFile(void)
{
	std::ifstream page(("pages" + _path).c_str());
	std::string body;
	std::string line;
	if (page.is_open())
	{
		while (getline(page, line))
		{
			body += line;
		}
	}
	return (body);
}

void Response::response(std::string request)
{
	(void)request;
	std::stringstream ss;
	ss << _statusCode;
	_response = getVersion()  + " " + ss.str() + " " + _status[_statusCode] + "\n";
	setContentType();
	_response += "Content-Type: " + _contentType + "\n";
	if (_statusCode != 404)
	{
		std::string body = readFile();
		ss.clear();
		std::stringstream ss;
		ss << body.length();
		_response += "Content-Length: " + ss.str() + "\n";
		_response += "\n" + body;
	}
}

std::ostream& operator<<(std::ostream &os, Response const &f)
{
	os << "Method : " << f.getMethod() << "\n";
	os << "Path : " << f.getPath() << "\n";
	os << "Version : " << f.getVersion() << "\n"; 
	os << "Response : " << f.getResponse();
	return (os);
}