/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/09 01:30:07 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/10 15:48:46 by aleite-b         ###   ########.fr       */
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
#include <sstream>
#include <fstream>
#include "Socket.hpp"
#include "Response.hpp"
#include "AutoIndex.hpp"
#include "RouteConfig.hpp"
#include "ServerConfig.hpp"
#include "WebServ.hpp"

void freeAll(std::vector<WebServ *> servers);
std::string trim(const std::string& str);
std::vector<std::string> split(const std::string &s, char delimiter);
void put_setting(std::string key, std::string value, std::string currentConfig, ServerConfig &config);
void put_setting_return(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config);
void put_setting_http(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config);
void put_setting_error_page(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config);
void getConfig(std::string path, std::vector<ServerConfig> &server_configs);
std::vector<std::string> split_whitespace(const std::string &s);