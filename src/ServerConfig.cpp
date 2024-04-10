#include "ServerConfig.hpp"

ServerConfig::ServerConfig(): port(8080), server_name("")
{

}

ServerConfig::~ServerConfig()
{
	
}

ServerConfig::ServerConfig(const ServerConfig &cpyServerConfig): port(cpyServerConfig.port), server_name(cpyServerConfig.server_name)
{
	for (std::map<std::string, RouteConfig>::const_iterator it = cpyServerConfig.routes.begin(); it != cpyServerConfig.routes.end(); ++it)
	{
        const std::string& path = it->first;
        const RouteConfig& originalRoute = it->second;
        RouteConfig copiedRoute = originalRoute;
        this->routes.insert(std::make_pair(path, copiedRoute));
    }
	for (std::map<int, std::string>::const_iterator it = cpyServerConfig.errors_pages.begin(); it != cpyServerConfig.errors_pages.end(); ++it)
	{
		const int& code = it->first;
		const std::string& msg = it->second;
		this->errors_pages.insert(std::make_pair(code, msg));
	}
}

ServerConfig &ServerConfig::operator=(const ServerConfig &cpyServerConfig)
{
	if (this != &cpyServerConfig)
	{
		this->port = cpyServerConfig.port;
		this->server_name = cpyServerConfig.server_name;
		for (std::map<std::string, RouteConfig>::const_iterator it = cpyServerConfig.routes.begin(); it != cpyServerConfig.routes.end(); ++it)
		{
			const std::string& path = it->first;
			const RouteConfig& originalRoute = it->second;
			RouteConfig copiedRoute = originalRoute;
			this->routes.insert(std::make_pair(path, copiedRoute));
		}
		for (std::map<int, std::string>::const_iterator it = cpyServerConfig.errors_pages.begin(); it != cpyServerConfig.errors_pages.end(); ++it)
		{
			const int& code = it->first;
			const std::string& msg = it->second;
			this->errors_pages.insert(std::make_pair(code, msg));
		}
	}
	return (*this);
}

bool ServerConfig::isRouteEmpty() const
{
	if (this->routes.empty())
		return true;
	return false;
}

void ServerConfig::insertRoute(std::string path, RouteConfig &route)
{
	this->routes.insert(std::make_pair(path, route));
}


void ServerConfig::setPort(int port)
{
	this->port = port;
}

void ServerConfig::setServerName(std::string server_name)
{
	this->server_name = server_name;
}

RouteConfig& ServerConfig::getOrSetRoute(std::string path)
{
    if (this->routes.find(path) == this->routes.end())
    {
        return this->routes[path];
    }
    else
    {
        return this->routes[path];
    }
}


void ServerConfig::setOneErrorPage(int return_code, std::string msg)
{
	this->errors_pages[return_code] = msg;
}

std::map<std::string, RouteConfig> &ServerConfig::getRoutes()
{
	return (this->routes);
}

std::map<int, std::string> &ServerConfig::getErrorsPages()
{
	return (this->errors_pages);
}

int ServerConfig::getPort() const
{
	return (this->port);
}

std::string ServerConfig::getServerName() const
{
	return (this->server_name);
}

void ServerConfig::clear() 
{
    this->routes.clear();
    this->errors_pages.clear();
}
