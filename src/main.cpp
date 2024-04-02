/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:19 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/02 15:38:53 by aleite-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

int	main(int argc, char **argv)
{
	std::string path;
    if (argc == 1)
        path = "./conf/webserv.conf";
    else if (argc == 2)
        path = argv[1];
    else
    {
        std::cerr << "Too much args" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<WebConfig> server_configs = getConfig(path);

    int epoll_fd = epoll_create(1);
    if (epoll_fd == -1) {
        std::cerr << "Error creating epoll: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    std::vector<WebServ *> servers;
    for (size_t i = 0; i < server_configs.size(); i++)
    {
        WebServ *new_serv = new WebServ(server_configs[i], epoll_fd);
        servers.push_back(new_serv);
    }

    int num_events = 0;
    std::map<int, Socket> client_fds;
    std::vector<epoll_event> events(20);
    while (true)
    {
        // std::cout << "------------------ Wait -------------------" << num_events << std::endl;
        num_events = epoll_wait(epoll_fd, events.data(), 20, 10);
        if (num_events == -1) {
            std::cerr << "Error in epoll_wait: " << std::strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        clock_t time_now = clock();
        bool have_timeout = false;
        for (std::map<int, Socket>::iterator it = client_fds.begin(); it != client_fds.end(); ++it)
        {
            time_now = clock();
            double diff_time = static_cast<double>(time_now - it->second.getTime()) / CLOCKS_PER_SEC;
            if (diff_time > 5.0)
            {
                close(it->first);
                client_fds.erase(it->first);
                have_timeout = true;
            }
        }
        if (have_timeout)
            continue;

        if (num_events < 1)
            continue;
        int valread;
        for (int i = 0; i < num_events; ++i) {
            int client_fd;
            std::vector<WebServ *>::iterator it;
            for (it = servers.begin(); it != servers.end(); it++)
            {
                WebServ* server = *it;
                if (events[i].data.fd == server->getServerFd()) {
                    server->setEpollFd(epoll_fd);
                    client_fd = server->create_client();
                    client_fds.insert(std::make_pair(client_fd, Socket(client_fd)));
                    break;
                }
            }
            if (it == servers.end())
                client_fd = events[i].data.fd;

            if ((client_fds[client_fd].getBuffer().find("\r\n\r\n") == std::string::npos) && (events[i].events & EPOLLIN))
            {
                char buffer[1024] = {0};
                valread = recv(client_fd, buffer, 1024, MSG_DONTWAIT);
                if (valread < 1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        std::cerr << "Socket Unblocked : " << std::endl;
                        continue;
                    }
                    std::cerr << "Error Reading : " << std::endl;
                    client_fds.erase(client_fd);
                    if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) < 0)
                    {
                        std::cerr << "Error Deleting EPOLL : " << std::strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    close(client_fd);
                    continue;
                }
                client_fds[client_fd].addToBuffer(buffer);
                std::cout << client_fds[client_fd].getBuffer() << " - " << valread << std::endl;
            }

            if (client_fds[client_fd].getBuffer().find("\r\n\r\n") != std::string::npos)
            {
                Response response;
                response.parseAll(client_fds[client_fd].getBuffer());
                response.checkOpenFile();
                response.response(client_fds[client_fd].getBuffer());
                std::cout << "Response:" << std::endl << response.getResponse() << std::endl;
                // std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 17\n\nHello from server\n";
                std::string hello = response.getResponse();
                if (send(client_fd, hello.c_str(), hello.length(), 0) != static_cast<long int>(hello.length()))
                {
                    std::cerr << "Error Sending : " << std::endl;
                }
                client_fds.erase(client_fd);
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL) < 0)
                {
                    std::cerr << "Error Deleting EPOLL : " << std::strerror(errno) << std::endl;
                    exit(EXIT_FAILURE);
                }
                close(client_fd);
            }
        }
    }
	return (0);
}