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
#include "../inc/Parsing.hpp"
#include <fcntl.h>
#include "../inc/Socket.hpp"

#define PORT 8080
#define TIMEOUT 10000

int main()
{
	int server_fd, new_socket;
	long valread;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	//std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 17\n\nHello from server\n";

	if ((server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		std::cerr << "Error Socket : " << std::strerror(errno) << std::endl;
		return 1;
	}

    // Socket non bloquant sur read
    int flags = fcntl(server_fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error Getting Socket Flags : " << std::strerror(errno) << std::endl;
        return 1;
    }
    if (fcntl(server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error Setting Socket Flags : " << std::strerror(errno) << std::endl;
        return 1;
    }

	std::memset(&address, 0, sizeof(address));
	address.sin_family = htonl(AF_INET);
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		std::cerr << "Error Binding : " << std::strerror(errno) << std::endl;
		return 1;
	}

	if (listen(server_fd, 100) < 0)
	{
		std::cerr << "Error Listening : " << std::strerror(errno) << std::endl;
		return 1;
	}

    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        std::cerr << "Error Creating Epoll : " << std::strerror(errno) << std::endl;
        return 1;
    }

    struct epoll_event event;
    event.events = EPOLLIN; // add write aussi
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1)
    {
        std::cerr << "Error Adding Epoll : " << std::strerror(errno) << std::endl;
        return 1;
    }

    std::vector<struct epoll_event> events;
    std::map<int, Socket> accepted_sockets;
    events.resize(20);
	while (1)
	{
        int num_events = epoll_wait(epoll_fd, events.data(), 20, TIMEOUT);
        if (num_events == -1)
        {
            std::cerr << "Error Waiting Epoll : " << std::strerror(errno) << std::endl;
            return 1;
        }
        // if (num_events > (int)events.size())
        // {
        //     events.resize(num_events);
        //     num_events = epoll_wait(epoll_fd, events.data(), num_events, -1);
        //     if (num_events == -1)
        //     {
        //         std::cerr << "Error Waiting Epoll : " << std::strerror(errno) << std::endl;
        //         return 1;
        //     }
        // }

        // timeout
        clock_t time_now = clock();
        bool have_timeout = false;
        for (std::map<int, Socket>::iterator it = accepted_sockets.begin(); it != accepted_sockets.end(); ++it)
        {
            time_now = clock();
            double diff_time = static_cast<double>(time_now - it->second.getTime()) / CLOCKS_PER_SEC;
            if (diff_time > 5.0)
            {
                close(it->first);
                accepted_sockets.erase(it->first);
                have_timeout = true;
            }
        }
        if (have_timeout)
            continue;

        if (events[0].data.fd == server_fd)
        {
            for (int i = 0; i < num_events; ++i)
            {
                std::cout << "\n+++++++ Waiting for new connection ++++++++\n" << std::endl;
                if (accepted_sockets.find(events[i].data.fd) != accepted_sockets.end())
                {
                    new_socket = events[i].data.fd;
                }
                else
                {
                    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
                    {
                        std::cerr << "Error Accepting : " << std::strerror(errno) << std::endl;
                        return 1;
                    }
                    accepted_sockets.insert(std::make_pair(new_socket, Socket(new_socket)));
                }
                char buffer[1024] = {0};
                valread = read(new_socket, buffer, 1024);
                if (valread < 1)
                {
                    std::cerr << "Error Reading : " << std::endl;
                    continue;
                }
                accepted_sockets[new_socket].addToBuffer(buffer);
                std::cout << accepted_sockets[new_socket].getBuffer() << " - " << valread << std::endl;

                if (accepted_sockets[new_socket].getBuffer().find("\r\n\r\n") != std::string::npos)
                {
                    // Parsing p;
                    // p.parseRequest(accepted_sockets[new_socket]);
                    // if (p.tryOpenPage(p.getPagePath()))
                    //     std::cout << "Status Code : 200" << std::endl;
                    // else
                    //     std::cout << "Status Code : 404" << std::endl;
                    // std::cout << p.getPagePath() << std::endl;
                    std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 17\n\nHello from server\n";
                    if (send(new_socket, hello.c_str(), hello.length(), 0) != static_cast<long int>(hello.length()))
                    {
                        std::cerr << "Error Sending : " << std::endl;
                    }
                    std::cout << "------------------Hello message sent-------------------" << std::endl;
                    close(new_socket);
                    accepted_sockets.erase(new_socket);
                }
            }
        }

	}
	return 0;
}