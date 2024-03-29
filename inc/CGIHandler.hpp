/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:07:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/29 23:22:10 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <map>
#include "Request.hpp"
#include <string.h>

class Request;

class CGIHandler
{
	private:
		char **_env;
		std::map<std::string, std::string> _envMap;
		std::string _cgiPath;
		char **_argv;
		int pipein[2];
		int pipeout[2];
	public:
		CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler& operator=(const CGIHandler &other);
		~CGIHandler();
		int execute(std::string file);
		void setCgiEnv(Request &req);
		void setCgiPath(std::string path);
};