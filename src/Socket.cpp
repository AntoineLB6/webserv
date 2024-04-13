/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:17:36 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 17:49:05 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(): fd(-1), server_fd(-1), bufferVector()
{
	time = clock();
}

Socket::Socket(int fd, int server_fd): fd(fd), server_fd(server_fd), bufferVector()
{
	time = clock();
}

Socket::~Socket()
{
	bufferVector.clear();
}

Socket::Socket(const Socket &cpySocket): fd(cpySocket.fd), server_fd(cpySocket.server_fd), bufferVector(cpySocket.bufferVector), time(cpySocket.time)
{

}

Socket &Socket::operator=(const Socket &cpySocket)
{
	if (this != &cpySocket)
	{
		bufferVector = cpySocket.bufferVector;
		time = cpySocket.time;
	}
	return (*this);
}

int Socket::getFd() const
{
	return (this->fd);
}

int Socket::getServerFd() const
{
	return (this->server_fd);
}

int Socket::getBufferSize() const
{
	return (this->bufferVector.size());
}

std::vector<char> Socket::getBufferVector(void) const
{
	std::vector<char> temp;
	if (this->bufferVector.empty())
	{
		std::cout << "NULL" << std::endl; 
		return (temp);
	}
	return (this->bufferVector);
}

clock_t Socket::getTime() const
{
	return (this->time);
}

void Socket::addToBuffer(char *bufferToAdd, size_t size)
{
	this->bufferVector.insert(bufferVector.end(), bufferToAdd, bufferToAdd + size);
}
