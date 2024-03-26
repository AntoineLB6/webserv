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

	public:
		WebServ();
		~WebServ();
		WebServ(const WebServ &cpyWebServ);
		WebServ &operator=(const WebServ &cpyWebServ);
		void create_server();
		void create_epoll();
		void start_server();
};
