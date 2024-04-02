/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:02:51 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/02 15:41:50 by aleite-b         ###   ########.fr       */
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
#include "Socket.hpp"
#include "Response.hpp"
#include "main.hpp"

#define PORT 8080
#define TIMEOUT 10000

class Socket;

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
		struct WebConfig config;

	public:
		WebServ(char *port);
		~WebServ();
		WebServ(const WebServ &cpyWebServ);
		WebServ &operator=(const WebServ &cpyWebServ);
		WebServ(struct WebConfig& config, int epoll_fd);
		void create_server();
		void create_epoll();
		int create_client();
		int getServerFd() const;
		int getOrCreateSocket();
		int getEpollFd() const;
		WebConfig getConfig() const;
		void setEpollFd(int epoll);
		void setConfig(WebConfig &config);
		bool is_valid_host(const char* host);
		void handleGET(Request &req);
		void handlePOST(Request &req);
		void handleDELETE(Request &req);
		void handleForm();
		void handleFileUploads();
		void handleCGI();
};
