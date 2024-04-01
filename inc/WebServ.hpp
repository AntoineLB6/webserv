/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/01 16:02:51 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/01 16:05:06 by lmoheyma         ###   ########.fr       */
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
		WebServ(struct WebConfig configToSet);
		~WebServ();
		WebServ(const WebServ &cpyWebServ);
		WebServ &operator=(const WebServ &cpyWebServ);
		void create_server(char *port);
		void create_server();
		void create_epoll();
		void start_server();
		int getServerFd() const;
		void handleGET(Request &req);
		void handlePOST(Request &req);
		void handleDELETE(Request &req);
		void handleForm();
		void handleFileUploads();
		void handleCGI();
};
