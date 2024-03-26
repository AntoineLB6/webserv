#include "Socket.hpp"

Socket::Socket(): fd(-1), buffer("")
{
	time = clock();
}

Socket::Socket(int fd): fd(fd), buffer("")
{
	time = clock();
}

Socket::~Socket()
{
	
}

Socket::Socket(const Socket &cpySocket): fd(cpySocket.fd), buffer(cpySocket.buffer), time(cpySocket.time)
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