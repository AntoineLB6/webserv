#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string>
#include <arpa/inet.h>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/epoll.h>
#include <vector>
#include <map>
#include <algorithm>
#include <fcntl.h>
#include "Socket.hpp"
#include "Response.hpp"


struct RouteConfig
{
    std::vector<std::string> limit_except_accepted;
    int return_code;
    std::string return_redirection;
    std::string root;
    bool autoindex;
    std::string default_page;
    std::string client_body_temp_path;
};

struct WebConfig
{
    int port;
    std::string server_name;
    long client_max_body_size;
    std::map<int, std::string> errors_pages;
    std::map<std::string, struct RouteConfig> routes;
};


void defaultRoute(struct RouteConfig *route_config);
struct WebConfig defaultConfig(struct WebConfig config);
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string &s, char delimiter);
void put_setting(std::string key, std::string value, std::string currentConfig, struct WebConfig *config);
void put_setting_return(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config);
void put_setting_http(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config);
void put_setting_error_page(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config);
std::vector<WebConfig> getConfig(std::string path);