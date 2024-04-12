/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:17:36 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 01:17:38 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

Socket::Socket(): fd(-1), server_fd(-1), buffer("")
{
	time = clock();
}

Socket::Socket(int fd, int server_fd): fd(fd), server_fd(server_fd), buffer("")
{
	time = clock();
}

Socket::~Socket()
{
	
}

Socket::Socket(const Socket &cpySocket): fd(cpySocket.fd), server_fd(cpySocket.server_fd), buffer(cpySocket.buffer), time(cpySocket.time)
{

}

Socket &Socket::operator=(const Socket &cpySocket)
{
	if (this != &cpySocket)
	{
		buffer = cpySocket.buffer;
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
	return (this->buffer.size());
}

std::string Socket::getBuffer() const
{
	return (this->buffer);
}

clock_t Socket::getTime() const
{
	return (this->time);
}

void Socket::addToBuffer(char *bufferToAdd)
{
	this->buffer += bufferToAdd;
}
