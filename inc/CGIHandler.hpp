/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:07:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 19:39:24 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <map>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <cstdio>
#include "main.hpp"

class ServerConfig;
class Request;

class CGIHandler
{
	private:
		std::string tmp;
		char **_env;
		std::map<std::string, std::string> _envMap;
		std::string _cgiPath;
	public:
		CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler& operator=(const CGIHandler &other);
		~CGIHandler();
		std::string execute(Request &req, std::string path);
		void setCgiEnv(Request &req, std::string path, ServerConfig config);
		void setCgiPath(std::string path);
		void printEnv(void) ;
};