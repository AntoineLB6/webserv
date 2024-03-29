/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 13:54:08 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/29 22:44:23 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Request.hpp"

Request::Request()
{
	
}

Request::Request(std::string request)
{
	_request = request;
	readFirstLine();
	fillHeaders();
}

Request::Request(const Request &other)
{
	(void)other;
}

Request::~Request()
{

}

Request& Request::operator=(const Request &other)
{
	(void)other;
	return (*this);
}

std::map<std::string, std::string> Request::getHeaders(void) const
{
	return (_headers);
}

std::string Request::getPath(void) const
{
	return (getHeaders()["Path"]);
}

std::string Request::getMethod(void) const
{
	return (getHeaders()["Method"]);
}

std::string Request::getContentType(void) const
{
	return (_contentType);
}

std::string Request::getPort(void) const
{
	return (_port);
}

void Request::setPort(void)
{
	size_t i = getHeaders()["Host"].find(':');
	_port = getHeaders()["Host"].substr(i);
}

void Request::setContentType(void)
{
	size_t j;
	std::string extension;

	j = getPath().find_last_of('.');
	if (j != std::string::npos)
	{
		extension = getPath().substr(j, getPath().length() - 1);
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

void Request::readFirstLine(void)
{
	size_t i;
	size_t j;
	
	i = _request.find_first_of(' ');
	_headers["Method"] = _request.substr(0, i);
	j = _request.find_first_of(' ', i + 1);
	_headers["Path"] = _request.substr(i + 1, j - i - 1);
	i = j;
	j = 0;
	j = _request.find_first_of('\r', i + 1);
	_headers["Version"] = _request.substr(i + 1, j - i - 1);
}

void Request::fillHeaders(void)
{
	size_t i;
	size_t pos;
	std::string tmp;
	
	i = _request.find_first_of('\n');
	i++;
	while (1)
	{
		pos = _request.find_first_of(':', i);
		if (pos != std::string::npos)
		{
			tmp = _request.substr(i, pos - i);
			_headers[tmp] = _request.substr(pos + 2, _request.find_first_of('\r') - i);
			int h = 0;
			while (_headers[tmp][h] != '\r')
				h++;
			_headers[tmp].erase(h);
			i = _request.find_first_of('\r', i);
		}
		else
		{
			_headers["Body"] = _request.substr(i + 2, _request.find_last_of('\n', i) - 1 - i);
			break ;
		}
		if (_request.substr(i, i + 4) == "\r\n\r\n")
			break ;

		i += 2;
	}
	setPort();
	setContentType();
}

void Request::printHeaders(void)
{
	std::map<std::string, std::string>::iterator it;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}