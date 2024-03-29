/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:06:41 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/28 17:14:59 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::CGIHandler()
{
	
}

CGIHandler::~CGIHandler()
{

}

void CGIHandler::setEnv()
{
	
}

int CGIHandler::execute(std::string file)
{
	int pid = fork();
	int fd = open();

	if (pid < 0)
		return (1);
	if (!pid)
	{
		
	}
}