#include <string>
#include <time.h>

class Socket
{
	private:
		const int fd;
		std::string buffer;
		clock_t time;

	public:
		Socket();
		Socket(int fd);
		~Socket();
		Socket(const Socket &cpySocket);
		Socket &operator=(const Socket &cpySocket);
		int getFd() const;
		std::string getBuffer() const;
		clock_t getTime() const;
		void addToBuffer(char *bufferToAdd);
};
