#include "Response.hpp"
#include "WebServ.hpp"

WebServ::WebServ()
{
	this->create_server();
	this->create_epoll();
}

WebServ::~WebServ()
{
	
}

WebServ::WebServ(const WebServ &cpyWebServ): server_fd(cpyWebServ.server_fd), address(cpyWebServ.address)
{

}

WebServ &WebServ::operator=(const WebServ &cpyWebServ)
{
	if (this != &cpyWebServ)
	{
		server_fd = cpyWebServ.server_fd;
		address = cpyWebServ.address;
	}
	return (*this);
}

void WebServ::create_server()
{
	if ((this->server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0)) < 0)
	{
		std::cerr << "Error Socket : " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
	int flags = fcntl(this->server_fd, F_GETFL, 0);
    if (flags == -1)
    {
        std::cerr << "Error Getting Socket Flags : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (fcntl(this->server_fd, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        std::cerr << "Error Setting Socket Flags : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
	std::memset(&this->address, 0, sizeof(this->address));
	this->address.sin_family = htonl(AF_INET);
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(PORT);

	if (bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
	{
		std::cerr << "Error Binding : " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

	if (listen(this->server_fd, 100) < 0)
	{
		std::cerr << "Error Listening : " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}
}

void WebServ::create_epoll()
{
	this->epoll_fd = epoll_create(1);
    if (epoll_fd == -1)
    {
        std::cerr << "Error Creating Epoll : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
	this->event.events = EPOLLIN | EPOLLOUT; // add write aussi
    this->event.data.fd = this->server_fd;
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &this->event) == -1)
    {
        std::cerr << "Error Adding Epoll : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
	this->events.resize(20);
}

void WebServ::start_server()
{
	long valread;
    int addrlen = sizeof(&address);

	while (1)
	{
        int num_events = epoll_wait(this->epoll_fd, this->events.data(), 20, TIMEOUT);
        if (num_events == -1)
        {
            std::cerr << "Error Waiting Epoll : " << std::strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
		clock_t time_now = clock();
        bool have_timeout = false;
        for (std::map<int, Socket>::iterator it = this->accepted_sockets.begin(); it != this->accepted_sockets.end(); ++it)
        {
            time_now = clock();
            double diff_time = static_cast<double>(time_now - it->second.getTime()) / CLOCKS_PER_SEC;
            if (diff_time > 5.0)
            {
                close(it->first);
                this->accepted_sockets.erase(it->first);
                have_timeout = true;
            }
        }
        if (have_timeout)
            continue;

        if (this->events[0].data.fd == this->server_fd)
        {
            for (int i = 0; i < num_events; ++i)
            {
                std::cout << "\n+++++++ Waiting for new connection ++++++++\n" << this->accepted_sockets.size() << std::endl;
                if (this->accepted_sockets.find(this->events[i].data.fd) != this->accepted_sockets.end())
                {
                    this->new_socket = this->events[i].data.fd;
                }
                else
                {
                    if ((this->new_socket = accept(this->server_fd, (struct sockaddr *)&this->address, (socklen_t *)&addrlen)) < 0)
                    {
                        std::cerr << "Error Accepting : " << std::strerror(errno) << std::endl;
                        exit(EXIT_FAILURE);
                    }
                    this->accepted_sockets.insert(std::make_pair(this->new_socket, Socket(this->new_socket)));
                }
                if (this->events[i].events & EPOLLIN)
                {
                    char buffer[1024] = {0};
                    valread = read(this->new_socket, buffer, 1024);
                    if (valread < 1)
                    {
                        std::cerr << "Error Reading : " << std::endl;
                        continue;
                    }
                    this->accepted_sockets[this->new_socket].addToBuffer(buffer);
                    std::cout << this->accepted_sockets[this->new_socket].getBuffer() << " - " << valread << std::endl;
                }
                if (this->accepted_sockets[this->new_socket].getBuffer().find("\r\n\r\n") != std::string::npos)
                {
                    Response response;
                    response.parseAll(this->accepted_sockets[this->new_socket].getBuffer());
                    response.checkOpenFile();
                    response.response(this->accepted_sockets[this->new_socket].getBuffer());
                    std::cout << "Response:" << std::endl << response.getResponse() << std::endl;
                    // std::string hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 17\n\nHello from server\n";
                    std::string hello = response.getResponse();
                    if (send(this->new_socket, hello.c_str(), hello.length(), 0) != static_cast<long int>(hello.length()))
                    {
                        std::cerr << "Error Sending : " << std::endl;
                    }
                    std::cout << "------------------Hello message sent-------------------" << std::endl;
                    close(this->new_socket);
                    this->accepted_sockets.erase(this->new_socket);
                }
            }
        }
	}
}