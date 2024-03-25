#include "Sockets.hpp"

Sockets::Sockets(): fd(-1), request("")
{
	time = clock();
}

Sockets::Sockets(int fd): fd(fd), request("")
{
	time = clock();
}


Sockets::Sockets(const Sockets &cpySocket): fd(cpySocket.fd), request(cpySocket.request), time(cpySocket.time)
{

}


Sockets &Sockets::operator=(const Sockets &cpySocket)
{
	if (this != &cpySocket)
	{
		request = cpySocket.request;
		time = cpySocket.time;
	}
}