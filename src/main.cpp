/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:19 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/28 16:20:50 by aleite-b         ###   ########.fr       */
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
    struct WebConfig config = getConfig(path);

    // std::cout << "----------------------" << std::endl;
    // std::cout << "Port : " << config.port << std::endl;
    // std::cout << "Server Name : " << config.server_name << std::endl;
    // std::cout << "client_max_body_size : " << config.client_max_body_size << std::endl;
    // std::cout << "routes : /" << std::endl;
    // struct RouteConfig test = config.routes["/"];
    // std::cout << "limit_except_accepted : " << std::endl;
    // for (size_t i = 0; i < test.limit_except_accepted.size(); i++)
    // {
    //     std::cout << test.limit_except_accepted[i] << std::endl;
    // }
    
    // std::cout << "return_code : " << test.return_code << std::endl;
    // std::cout << "return_redirection : " << test.return_redirection << std::endl;
    // std::cout << "root : " << test.root << std::endl;
    // std::cout << "autoindex : " << test.autoindex << std::endl;
    // std::cout << "default_page : " << test.default_page << std::endl;
    // std::cout << "client_body_temp_path : " << test.client_body_temp_path << std::endl;
    
    // WebServ server(argv[1]);
    WebServ server(config);
    server.start_server();
	return (0);
}