#include <string>
#include <time.h>

class Sockets
{
	private:
		const int fd;
		std::string request;
		clock_t time;
		Sockets();

	public:
		Sockets(int fd);
		~Sockets();
		Sockets(const Sockets &cpySocket);
		Sockets &operator=(const Sockets &cpySocket);
};
