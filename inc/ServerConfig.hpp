#pragma once

#include <string>
#include <time.h>
#include <map>
#include "main.hpp"

class RouteConfig;

class ServerConfig
{
	private:
		int port;
		std::string server_name;
		std::map<int, std::string> errors_pages;
		std::map<std::string, RouteConfig> routes;

	public:
		ServerConfig();
		~ServerConfig();
		ServerConfig(const ServerConfig &cpyServerConfig);
		ServerConfig &operator=(const ServerConfig &cpyServerConfig);
		bool isRouteEmpty() const;
		void insertRoute(std::string path, RouteConfig &route);
		void setPort(int port);
		void setServerName(std::string server_name);
		RouteConfig &getOrSetRoute(std::string path);
		void setOneErrorPage(int return_code, std::string msg);
		std::map<std::string, RouteConfig> &getRoutes();
		std::map<int, std::string> &getErrorsPages();
		int getPort() const;
		std::string getServerName() const;
		void clear();
};
