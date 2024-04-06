/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 13:54:08 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/06 17:10:28 by lmoheyma         ###   ########.fr       */
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

std::string Request::getVersion(void) const
{
	return (getHeaders()["Version"]);
}

std::string Request::getPath(void) const
{
	return (getHeaders()["Path"]);
}

std::string Request::getMethod(void) const
{
	return (getHeaders()["Method"]);
}

std::string Request::getBody(void) const
{
	return (getHeaders()["Body"]);
}

std::string Request::getContentType(void) const
{
	std::map<std::string, std::string>::iterator it;
	std::map<std::string, std::string> map = getHeaders();

	it = map.find("Content-Type");
	if (it != map.end() && it->second.find("form") == std::string::npos)
	{
		std::cout << "Content type map : ===============    " << std::endl;
		return (map["Content-Type"]);
	}
	return (_contentType);
}

std::string Request::getPort(void) const
{
	return (_port);
}

std::string Request::getRequest(void) const
{
	return (_request);
}

void Request::setPort(void)
{
	size_t i = getHeaders()["Host"].find(':');
	_port = getHeaders()["Host"].substr(i + 1);
}

std::string Request::getContentLength(void)
{
	std::stringstream ss;
	ss << getHeaders()["Body"].length();
	_contentLength = ss.str();

	return (_contentLength);
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
		_contentType = "text/html";
}

void Request::readFirstLine(void)
{
	size_t i;
	size_t j;
	size_t k;
	std::string extension;
	
	i = _request.find_first_of(' ');
	_headers["Method"] = _request.substr(0, i);
	j = _request.find_first_of(' ', i + 1);
	_headers["Path"] = _request.substr(i + 1, j - i - 1);
	if ((k = _headers["Path"].find_last_of('.')) == std::string::npos)
		k = 0;
	extension = _headers["Path"].substr(k, _headers["Path"].length() - 1);
	if (extension.find("?") != std::string::npos)
	{
		_headers["Body"] = _headers["Path"].substr(_headers["Path"].find_first_of("?") + 1, _headers["Path"].length() - 1); 
		_headers["Path"] = _headers["Path"].substr(0, _headers["Path"].find("?"));
	}
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
			size_t j = 0;
			while (_headers[tmp][j] != '\r')
				j++;
			_headers[tmp].erase(j);
			i = _request.find_first_of('\r', i);
		}
		else
		{
			if (_headers["Body"].empty())
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
	std::cout << std::endl << "REQUEST HEADERS: " << std::endl;
	for (it = _headers.begin(); it != _headers.end(); it++)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
}