/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/25 20:18:18 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parsing.hpp"

Parsing::Parsing()
{

}

Parsing::~Parsing()
{

}

std::string Parsing::getPagePath(void) const 
{
	return (pagePath);
}

void Parsing::parseRequest(std::string request)
{
	size_t pos = request.find("Referer");
	if (pos != std::string::npos)
	{
		int i = 0;
		int count = 0;
		request = strstr(request.c_str(), "Referer");
		while (request[i] && count != 3)
		{
			if (request[i] && request[i] == '/')
				count++;
			i++;
		}
		int end = i;
		while (request[end] != '\r')
			end++;
		pagePath = request.substr(i, end);
		// pagePath = strtok((char *)request.c_str(), "\r\n");
		printf("Page path : [%s]\n", pagePath.c_str());
	}
	else
	{
		request = strstr(request.c_str(), "/");
		pagePath = strtok((char *)request.c_str(), " ");
		printf("Page path : [%s]\n", pagePath.c_str());
	}
}

bool Parsing::tryOpenPage(std::string pageName)
{
	std::string path = "pages";
	pagePath = strcat((char *)path.c_str(), pageName.c_str()); 
	std::ifstream page (pagePath.c_str());
	return (page.is_open());
}