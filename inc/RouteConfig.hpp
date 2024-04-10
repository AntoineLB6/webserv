#pragma once

#include <string>
#include <time.h>
#include <map>
#include <vector>

class RouteConfig
{
	private:
		std::string root;
		bool autoindex;
		std::string default_page;
		std::string client_body_temp_path;
		long client_max_body_size;
		std::vector<std::string> limit_except_accepted;
		std::map<int, std::string> return_codes;

	public:
		RouteConfig();
		~RouteConfig();
		RouteConfig(const RouteConfig &cpyRouteConfig);
		RouteConfig &operator=(const RouteConfig &cpyRouteConfig);
		void setRoot(std::string root);
		void setAutoindex(bool autoindex);
		void setDefault(std::string defaultval);
		void setClientBodyTempPath(std::string client_body_temp_path);
		void setClientMaxBodySize(long client_max_body_size);
		void setOneReturnCode(int return_code, std::string msg);
		void addOneMethod(std::string method);
		bool isMethodsEmpty() const;
		std::string getRoot() const;
		bool getAutoindex() const;
		std::string getDefaultPage() const;
		std::string getClientBodyTempPath() const;
		long getClientMaxBodySize() const;
		std::vector<std::string> &getLimitExceptAccepted();
		std::map<int, std::string> &getReturnCodes();
};
