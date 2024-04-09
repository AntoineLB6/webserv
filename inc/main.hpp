/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 01:30:07 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/09 17:11:42 by aleite-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include <sys/stat.h>
#include <dirent.h>
#include <map>
#include <algorithm>
#include <fcntl.h>
#include "Socket.hpp"
#include "Response.hpp"
#include "AutoIndex.hpp"

struct RouteConfig
{
    std::vector<std::string> limit_except_accepted;
    std::map<int, std::string> return_codes;
    std::string root;
    bool autoindex;
    std::string default_page;
    std::string client_body_temp_path;
    long client_max_body_size;
};

struct WebConfig
{
    int port;
    std::string server_name;
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
std::vector<std::string> split_whitespace(const std::string &s);