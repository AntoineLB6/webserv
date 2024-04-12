/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RouteConfig.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:17:49 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 01:17:51 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "RouteConfig.hpp"

RouteConfig::RouteConfig(): root("./pages"), autoindex(true), default_page("index.html"), client_body_temp_path("./uploads"), client_max_body_size(1024)
{

}

RouteConfig::~RouteConfig()
{
	
}

RouteConfig::RouteConfig(const RouteConfig &cpyRouteConfig): root(cpyRouteConfig.root), autoindex(cpyRouteConfig.autoindex), default_page(cpyRouteConfig.default_page), client_body_temp_path(cpyRouteConfig.client_body_temp_path), client_max_body_size(cpyRouteConfig.client_max_body_size)
{
		for (std::map<int, std::string>::const_iterator it = cpyRouteConfig.return_codes.begin(); it != cpyRouteConfig.return_codes.end(); ++it)
		{
			const int& code = it->first;
			const std::string& msg = it->second;
			this->return_codes.insert(std::make_pair(code, msg));
		}
		for (std::vector<std::string>::const_iterator it = cpyRouteConfig.limit_except_accepted.begin(); it != cpyRouteConfig.limit_except_accepted.end(); ++it)
		{
			const std::string& msg = *it;
			this->limit_except_accepted.push_back(msg);
		}
}

RouteConfig &RouteConfig::operator=(const RouteConfig &cpyRouteConfig)
{
	if (this != &cpyRouteConfig)
	{
		this->root = cpyRouteConfig.root;
		this->autoindex = cpyRouteConfig.autoindex;
		this->default_page = cpyRouteConfig.default_page;
		this->client_body_temp_path = cpyRouteConfig.client_body_temp_path;
		this->client_max_body_size = cpyRouteConfig.client_max_body_size;
		for (std::map<int, std::string>::const_iterator it = cpyRouteConfig.return_codes.begin(); it != cpyRouteConfig.return_codes.end(); ++it)
		{
			const int& code = it->first;
			const std::string& msg = it->second;
			this->return_codes.insert(std::make_pair(code, msg));
		}
		for (std::vector<std::string>::const_iterator it = cpyRouteConfig.limit_except_accepted.begin(); it != cpyRouteConfig.limit_except_accepted.end(); ++it)
		{
			const std::string& msg = *it;
			this->limit_except_accepted.push_back(msg);
		}
	}
	return (*this);
}

void RouteConfig::setRoot(std::string root)
{
	this->root = root;
}

void RouteConfig::setAutoindex(bool autoindex)
{
	this->autoindex = autoindex;
}

void RouteConfig::setDefault(std::string defaultval)
{
	this->default_page = defaultval;
}

void RouteConfig::setClientBodyTempPath(std::string client_body_temp_path)
{
	this->client_body_temp_path = client_body_temp_path;
}

void RouteConfig::setClientMaxBodySize(long client_max_body_size)
{
	this->client_max_body_size = client_max_body_size;
}

void RouteConfig::setOneReturnCode(int return_code, std::string msg)
{
	this->return_codes[return_code] = msg;
}

void RouteConfig::addOneMethod(std::string method)
{
	this->limit_except_accepted.push_back(method);
}

bool RouteConfig::isMethodsEmpty() const
{
	if (this->limit_except_accepted.empty())
		return true;
	return false;
}

std::string RouteConfig::getRoot() const
{
	return (this->root);
}

bool RouteConfig::getAutoindex() const
{
	return (this->autoindex);
}

std::string RouteConfig::getDefaultPage() const
{
	return (this->default_page);
}

std::string RouteConfig::getClientBodyTempPath() const
{
	return (this->client_body_temp_path);
}

long RouteConfig::getClientMaxBodySize() const
{
	return (this->client_max_body_size);
}

std::vector<std::string> &RouteConfig::getLimitExceptAccepted()
{
	return (this->limit_except_accepted);
}

std::map<int, std::string> &RouteConfig::getReturnCodes()
{
	return (this->return_codes);
}
