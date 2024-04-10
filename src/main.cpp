/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:19 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/10 15:41:18 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

void deleteAllConfigs(std::vector<WebConfig>& server_configs)
{
    for (std::vector<WebConfig>::iterator it = server_configs.begin(); it != server_configs.end(); it++)
    {
        // WebConfig& config = *it;
        // std::map<std::string, RouteConfig*> routes = config.routes;
        // for (std::map<std::string, RouteConfig*>::iterator routes_it = routes.begin(); routes_it != routes.end(); ++routes_it)
        // {
        //     delete routes_it->second; // Supprimer l'objet pointé
        // }
        // routes.clear();
    }
}

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
    










    
    std::cout << "==========================================" << std::endl;
    int i = 1;
    for (std::vector<WebConfig>::iterator it = server_configs.begin(); it != server_configs.end(); it++)
    {
        std::cout << "Config " << std::endl;
        WebConfig config = *it;

        std::cout << "Port : " << config.port << std::endl;
        std::cout << "Server Name : " << config.server_name << std::endl;

        std::cout << "Errors pages : ";
        for (std::map<int, std::string>::iterator it = config.errors_pages.begin(); it != config.errors_pages.end(); ++it)
        {
            std::cout << it->first << " " << it->second << " | ";
        }
        std::cout << std::endl;
        
        for (std::map<std::string, RouteConfig>::iterator it = config.routes.begin(); it != config.routes.end(); it++)
        {
            RouteConfig route = it->second;
            std::cout << "==== Chemin : " << it->first << std::endl;
            std::cout << "--Returned Codes : ";
            for (std::map<int, std::string>::iterator it = route.return_codes.begin(); it != route.return_codes.end(); ++it)
            {
                std::cout << it->first << " " << it->second << " | ";
            }
            std::cout << std::endl;
            std::cout << "Root : " << route.root << std::endl;
            std::cout << "Autoindex : " << route.autoindex << std::endl;
            std::cout << "Default Page : " << route.default_page << std::endl;
            std::cout << "Client body install repo : " << route.client_body_temp_path << std::endl;
            std::cout << "Max body size : " << route.client_max_body_size << std::endl;
            std::cout << "Allowed Methods : ";
            for (std::vector<std::string>::iterator it = route.limit_except_accepted.begin(); it != route.limit_except_accepted.end(); it++)
            {
                std::cout << *it;
            }
            std::cout << std::endl;
        }
        i++;
        std::cout << std::endl;
        std::cout << std::endl;
    }
    



    for (std::vector<WebConfig>::iterator it = server_configs.begin(); it != server_configs.end(); it++)
    {
        WebConfig config = *it;
        for (std::vector<WebConfig>::iterator it2 = server_configs.begin(); it2 != server_configs.end(); it2++)
        {
            WebConfig config2 = *it2;
            if (it == it2)
                continue;
            if (config.server_name == config2.server_name)
            {
                deleteAllConfigs(server_configs);
                std::cerr << "Error same hostnames." << std::endl;
                exit(EXIT_FAILURE);
            }

        }
    }


    

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
        num_events = epoll_wait(epoll_fd, events.data(), 20, -1);
        if (num_events == -1) {
            std::cerr << "Error in epoll_wait: " << std::strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }

        clock_t time_now = clock();
        bool have_timeout = false;

        for (std::map<int, Socket>::iterator it = client_fds.begin(); it != client_fds.end(); it++)
        {
            time_now = clock();
            double diff_time = static_cast<double>(time_now - it->second.getTime()) / CLOCKS_PER_SEC;
            if (diff_time > 5.0)
            {
                client_fds.erase(it->first);
                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, it->first, NULL) < 0)
                {
                    std::cerr << "Error Deleting EPOLL : " << std::strerror(errno) << std::endl;
                    exit(EXIT_FAILURE);
                }
                close(it->first);
                have_timeout = true;
                break;
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
                    client_fds.insert(std::make_pair(client_fd, Socket(client_fd, server->getServerFd())));
                    break;
                }
            }
            if (it == servers.end())
                client_fd = events[i].data.fd;
            
            // size_t pos = client_fds[client_fd].getBuffer().find("\r\n\r\n");
            if ((events[i].events & EPOLLIN))
            {
                    char buffer[1024] = {0};
                    valread = recv(client_fd, buffer, sizeof(buffer), MSG_DONTWAIT);
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



                    // std::cout << "=====================================" << std::endl;
                    std::cout << buffer << std::endl;
                    // std::cout << "=====================================" << std::endl;
    
            }
            
            // pos = client_fds[client_fd].getBuffer().find("\r\n\r\n");
            if (!(events[i].events & EPOLLIN) && (events[i].events & EPOLLOUT) && valread > 0)
            {
                    Request req(client_fds[client_fd].getBuffer());
                    std::string response;
                    for (it = servers.begin(); it != servers.end(); it++)
                    {
                        WebServ* server = *it;
                        if (client_fds[client_fd].getServerFd() == server->getServerFd())
                        {
                            WebConfig config = server->getConfig();
                            struct RouteConfig route;
                            
                            if (config.routes.find(req.getPath()) != config.routes.end())
                            {
                                route = config.routes.find(req.getPath())->second;
                            }
                            else
                            {
                                route = config.routes.find("/")->second;
                            }
                            
                            if (req.getMethod() == "GET" && (std::find(route.limit_except_accepted.begin(), route.limit_except_accepted.end(), req.getMethod()) != route.limit_except_accepted.end()))
                            {
                                response = handleGET(req, route, config);
                            }
                            else if (req.getMethod() == "POST" && (std::find(route.limit_except_accepted.begin(), route.limit_except_accepted.end(), req.getMethod()) != route.limit_except_accepted.end()))
                            {
                                response = handlePOST(req, route, config);
                            }
                            else if (req.getMethod() == "DELETE" && (std::find(route.limit_except_accepted.begin(), route.limit_except_accepted.end(), req.getMethod()) != route.limit_except_accepted.end()))
                                response = handleDELETE(req, route, config);
                            else
                            {
                                Response rep;
                                response = getErrorsPages("501", route, config, rep);
                            }
                            break;
                        }
                    }
                    std::cout << std::endl << std::endl << "Response: \n"<< response << std::endl;
                    if (send(client_fd, response.c_str(), response.length(), 0) != static_cast<long int>(response.length()))
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