/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:02:51 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 01:00:34 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
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
#include "main.hpp"

#define PORT 8080
#define TIMEOUT 10000

class Socket;
class Response;
class ServerConfig;
class Request;

class WebServ
{
	private:
		int server_fd;
		struct sockaddr_in address;
		int epoll_fd;
		struct epoll_event event;
		std::vector<struct epoll_event> events;
    	std::map<int, Socket> accepted_sockets;
		int new_socket;
		ServerConfig& config;
		std::vector<WebServ *> servers;

	public:
		WebServ(char *port);
		~WebServ();
		WebServ(const WebServ &cpyWebServ);
		WebServ &operator=(const WebServ &cpyWebServ);
		WebServ(ServerConfig& config, int epoll_fd, std::vector<WebServ *> servers);
		void create_server();
		void create_epoll();
		int create_client();
		int getServerFd() const;
		int getOrCreateSocket();
		int getEpollFd() const;
		ServerConfig getConfig();
		void setEpollFd(int epoll);
		void setConfig(ServerConfig &config);
		bool is_valid_host(const char* host);
};

std::string handleGET(Request &req, RouteConfig route, ServerConfig config);
std::string handlePOST(Request &req, RouteConfig route, ServerConfig config);
std::string handleDELETE(Request &req, RouteConfig route, ServerConfig config);
std::string handleFileUploads(Request &req , RouteConfig route, ServerConfig config, Response &response);
std::string handleCGI(Request &req, Response &response, RouteConfig route, ServerConfig config);
std::string getErrorsPages(std::string code, RouteConfig route, ServerConfig config, Response &response);
std::string contentLength(Response &response);
std::string getDisplayDate(void);