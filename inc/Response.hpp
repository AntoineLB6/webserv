/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:28 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/30 22:02:02 by lmoheyma         ###   ########.fr       */
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

class CGIHandler;

class Response
{
	private:
		CGIHandler _cgi;
		size_t i;
		std::string _method;
		std::string _path;
		std::string _version;
		std::string _response;
		std::string _connection;
		std::string _contentType;
		std::string _date;
		std::map<std::string, std::string> _header;
		size_t _statusCode;
		int cgiFd[2];
		std::map<int, std::string> _status;
	public:
		Response();
		Response(const Response &other);
		~Response();
		Response& operator=(const Response &other);
		
		// Parsing First Line
		void parseMethod(std::string request);
		void parsePath(std::string request);
		void parseVersion(std::string request);
		void parseAll(std::string request, Request &req);

		//Setters
		void setStatusCode(int statusCode);
		void setContentType(void);
		void setDate(void);
		std::string handleCGI(Request &req);

		// Getters
		std::string getMethod(void) const ;
		std::string getPath(void) const ;
		std::string getVersion(void) const ;
		std::string getResponse(void) const ;

		// Response
		void checkOpenFile(void) ;
		void response(std::string request);
		std::string readFile(std::string code);
		void chooseResponse(std::string request, Request &req);
};

std::ostream& operator<<(std::ostream &os, Response const &f);