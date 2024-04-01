/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 13:54:17 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/01 16:07:52 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>
#include <iterator>
#include <sstream>
#include "Colors.hpp"

class Request
{
	private:
		std::map<std::string, std::string> _headers;
		std::string _request;
		std::string _contentType;
		std::string _port;
		std::string _contentLength;
	public:
		Request();
		Request(std::string request);
		Request(const Request &other);
		~Request();
		Request& operator=(const Request &other);
		std::map<std::string, std::string> getHeaders(void) const;
		void readFirstLine(void);
		void fillHeaders(void);
		void printHeaders(void);
		void setContentType(void);
		void setPort(void);
		std::string getPath(void) const ;
		std::string getMethod(void) const ;
		std::string getContentType(void) const ;
		std::string getPort(void) const ;
		std::string getContentLength(void) ;
		std::string getRequest(void) const ;
};