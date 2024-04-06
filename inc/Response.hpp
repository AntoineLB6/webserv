/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:28 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/06 16:42:42 by aleite-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <cstring>
#include <map>
#include <sstream>
#include <unistd.h>
#include "CGIHandler.hpp"
#include "Request.hpp"
#include "AutoIndex.hpp"
#include <sys/stat.h>

class CGIHandler;
class AutoIndex;

class Response
{
	private:
		CGIHandler _cgi;
		size_t i;
		std::string _method;
		std::string _path;
		std::string _response;
		std::string _connection;
		std::string _contentType;
		std::map<std::string, std::string> _header;
		size_t _statusCode;
		int cgiFd[2];
		std::map<int, std::string> _status;
		bool _is_dir;
		bool _is_autoindex;
	public:
		Response();
		Response(const Response &other);
		~Response();
		Response& operator=(const Response &other);
		
		// Setters
		void setVersion(std::string version);
		void setContentType(std::string contentType);
		void setContentLength(std::string contentLength);
		void setCGIContentLength(std::string path);
		void setHeaders(Request &req, int flag, std::string cgiBody, struct RouteConfig route);
		void setStatusCode(int statusCode);
		void setDate(void);
		void setServer(std::string serverName);
		void setConnection(std::string connection, Request &req);
		void setBody(std::string code, std::string path);
		std::string handleCGI(Request &req);

		// Getters
		std::string getResponse(void) const ;
		std::string getStatusCode(void) const ;

		// Response
		void checkOpenFile(std::string path, Request &req, struct RouteConfig route) ;
		void openDirectory(struct RouteConfig route);
		void openListTree();
		void response(std::string request);
		std::string readFile(std::string code, std::string path);
		int CGIBodyLength(std::string cgiBody);
};

std::ostream& operator<<(std::ostream &os, Response const &f);
int isDirectoryOrIndex(const std::string& path);