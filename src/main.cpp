/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:19 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/27 17:38:21 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

int main(int argc, char **argv)
{
    if (argc == 2)
    {
        WebServ server(argv[1]);
        server.start_server();
    }
	return 0;
}