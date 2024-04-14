/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:07 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/14 16:45:41 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebServ.hpp"

WebServ::WebServ(ServerConfig& config, int epoll_fd_to_set, std::vector<WebServ *> servers) : config(config), servers(servers)
{
    this->epoll_fd = epoll_fd_to_set;
    this->create_server();
	this->create_epoll();
}

WebServ::~WebServ()
{
	
}

WebServ::WebServ(const WebServ &cpyWebServ): server_fd(cpyWebServ.server_fd), address(cpyWebServ.address), config(cpyWebServ.config)
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
		freeAll(this->servers);
        exit(EXIT_FAILURE);
	}
    std::cout << BOLDCYAN << "[" << getDisplayDate() << "] " << BOLDGREEN
            << "server : starting " << RESET << std::endl;
    const int enable = 1;
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::cerr << "Error Socket Option : " << std::strerror(errno) << std::endl;
		freeAll(this->servers);
        exit(EXIT_FAILURE);
	}

    if (fcntl(this->server_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error Setting Socket Flags : " << std::strerror(errno) << std::endl;
        freeAll(this->servers);
        exit(EXIT_FAILURE);
    }
	
    // if (!this->config.server_name.empty())
	// {
	// 	if (!this->is_valid_host(config.server_name.c_str()) || (config.port < 0 || config.port >= 65535))
	// 	{
	// 		std::cerr << "Error in host & server_name params : " << std::strerror(errno) << std::endl;
	// 		exit(EXIT_FAILURE);
	// 	}
	// }

	std::memset(&this->address, 0, sizeof(this->address));
	this->address.sin_family = AF_INET;
	this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(config.getPort());

	if (bind(this->server_fd, (struct sockaddr *)&this->address, sizeof(this->address)) < 0)
	{
		std::cerr << "Error Binding : " << std::strerror(errno) << std::endl;
		freeAll(this->servers);
        exit(EXIT_FAILURE);
	}

	if (listen(this->server_fd, 100) < 0)
	{
		std::cerr << "Error Listening : " << std::strerror(errno) << std::endl;
		freeAll(this->servers);
        exit(EXIT_FAILURE);
	}
}

void WebServ::create_epoll()
{
	this->event.events = EPOLLIN | EPOLLOUT;
    this->event.data.fd = this->server_fd;
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &this->event) == -1)
    {
        std::cerr << "Error Adding Epoll : " << std::strerror(errno) << std::endl;
        freeAll(this->servers);
        exit(EXIT_FAILURE);
    }
}

int WebServ::create_client()
{
    int addrlen = sizeof(&address);
    int client_fd;

    if ((client_fd = accept(this->server_fd, (struct sockaddr *)&this->address, (socklen_t *)&addrlen)) < 0) {
        std::cerr << "Error Accepting : " << std::strerror(errno) << std::endl;
        freeAll(this->servers);
        exit(EXIT_FAILURE);
    }
    std::cout << BOLDCYAN << "[" << getDisplayDate() << "] " << BOLDGREEN 
            << "server : new connection on port " << config.getPort() << RESET << std::endl;
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Error Setting Socket Flags client : " << std::strerror(errno) << std::endl;
        freeAll(this->servers);
        exit(EXIT_FAILURE);
    }

    struct epoll_event client_event;

    client_event.events = EPOLLIN | EPOLLOUT;
    client_event.data.fd = client_fd;
    
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0)
    {
        std::cerr << "Error Accepting EPOLL : " << std::strerror(errno) << std::endl;
        freeAll(this->servers);
        exit(EXIT_FAILURE);
    }
    return client_fd;
}

int findCGIExtension(std::string path)
{
    return (path.find(".php") != std::string::npos || path.find(".py") != std::string::npos);
}

std::string handleGET(Request &req, RouteConfig route, ServerConfig config)
{
    Response response(config);
    std::string cgiBody = "";
    std::string rep;
    std::string tempPath;

	if (static_cast<long>(req.getBody().size()) > route.getClientMaxBodySize())
		return (getErrorsPages("413", route, config, response));
    if (req.getPath() != "/")
    {
        tempPath = req.getPath().substr(0, req.getPath().find_last_of("/") + 1);
        if (config.getRoutes().find(tempPath) != config.getRoutes().end() && findCGIExtension(req.getPath()))
        {
            cgiBody = handleCGI(req, response, route, config);
            if (cgiBody.empty())
                return (getErrorsPages("501", route, config, response));
        }
        else if (config.getRoutes().find(tempPath) == config.getRoutes().end() && findCGIExtension(req.getPath()))
            return (getErrorsPages("501", route, config, response));
    }
    if (config.getRoutes().find(tempPath) != config.getRoutes().end() && findCGIExtension(req.getPath()))
        response.setHeaders(req, 1, cgiBody, route, config);
    else
        response.setHeaders(req, 0, cgiBody, route, config);
    if (!cgiBody.empty())
        rep = response.getResponse() + cgiBody;
    else
        rep = response.getResponse();
    std::cout << BOLDCYAN << "[" << getDisplayDate() << "] " << BOLDMAGENTA << "server : "
            << BOLDWHITE << ">> " << BOLDMAGENTA << "[status: " << response.getStatusCode() 
            << " " << response.getErrorsPages()[std::atoi(response.getStatusCode().c_str())]
            << "] [length: " << contentLength(response) << "]" << RESET << std::endl;
    return (rep);
}

std::string handlePOST(Request &req, RouteConfig route, ServerConfig config)
{
    Response response(config);
    std::string cgiBody = "";
    std::string rep;
    std::string tempPath = req.getPath().substr(0, req.getPath().find_last_of("/") + 1);
    
    response.setStatus(route);
	if (static_cast<long>(req.getBody().size()) > route.getClientMaxBodySize())
		return (getErrorsPages("413", route, config, response));
    if (req.getPath() != "/")
    {
        if (config.getRoutes().find(tempPath) != config.getRoutes().end() && findCGIExtension(req.getPath()))
        {
            cgiBody = handleCGI(req, response, route, config);
            if (cgiBody.empty())
                return (getErrorsPages("501", route, config, response));
        }
        else if (config.getRoutes().find(tempPath) == config.getRoutes().end() && findCGIExtension(req.getPath()))
            return (getErrorsPages("501", route, config, response));
    }
    if (config.getRoutes().find(tempPath) != config.getRoutes().end() && findCGIExtension(req.getPath()))
        response.setHeaders(req, 1, cgiBody, route, config);
    else
        response.setHeaders(req, 0, cgiBody, route, config);
    if (!cgiBody.empty())
        rep = response.getResponse() + cgiBody;
    else
        rep = response.getResponse();
    if (response.getStatusCode() == "200")
    {
        std::string contentType = req.getContentType(config);
        contentType = contentType.substr(0, contentType.find(";"));
        if (contentType == "multipart/form-data")
        {
            std::string uploadRep = handleFileUploads(req, route, config, response);
            if (!uploadRep.empty())
                rep = uploadRep;
        }
    }
    std::cout << BOLDCYAN << "[" << getDisplayDate() << "] " << BOLDMAGENTA << "server : "
            << BOLDWHITE << ">> " << BOLDMAGENTA << "[status: " << response.getStatusCode() 
            << " " << response.getErrorsPages()[std::atoi(response.getStatusCode().c_str())]
            << "] [length: " << contentLength(response) << "]" << RESET << std::endl;
    return (rep);
}

bool isDirectory(const std::string& path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
    {
        if (!path.empty() && path[path.size() - 1] == '/')
        {
        	return true;
        }
		return false;
	}
	return S_ISDIR(info.st_mode);
}

std::string handleDELETE(Request &req, RouteConfig route, ServerConfig config)
{
    Response response(config);
    std::string cgiBody = "";

	if (static_cast<long>(req.getBody().size()) > route.getClientMaxBodySize())
		return (getErrorsPages("413", route, config, response));
    std::string path = route.getRoot() + req.getPath();
    if (isDirectory(path))
        response.setHeaders(req, 403, cgiBody, route, config);
    else
    {
        if (std::remove(path.c_str()) != 0)
            response.setHeaders(req, 404, cgiBody, route, config);
        else
            response.setHeaders(req, 200, cgiBody, route, config);
    }
    std::cout << BOLDCYAN << "[" << getDisplayDate() << "] " << BOLDMAGENTA << "server : "
            << BOLDWHITE << ">> " << BOLDMAGENTA << "[status: " << response.getStatusCode() 
            << " " << response.getErrorsPages()[std::atoi(response.getStatusCode().c_str())]
            << "] [length: " << contentLength(response) << "]" << RESET << std::endl;
    return (response.getResponse());
}

std::string handleFileUploads(Request &req, RouteConfig route, ServerConfig config, Response &response)
{
    std::ofstream outfile((route.getClientBodyTempPath() + req.getFilename()).c_str());
    std::string rep;

    if (req.getFilename().empty() || req.getFilename().length() == 0)
    {
        rep = getErrorsPages("400", route, config, response);
        return (rep);
    }
    if (outfile.is_open())
    {
        if (!req.getBody().empty())
            outfile << req.getBody();
        outfile.close();
    }
    else
    {
        rep = getErrorsPages("404", route, config, response);
    }
    return (rep);
}

std::string handleCGI(Request &req, Response &response, RouteConfig route, ServerConfig config)
{
    std::string body = response.handleCGI(req, route, config);
    return (body);
}

std::string getErrorsPages(std::string code, RouteConfig route, ServerConfig config, Response &response)
{
    std::ifstream page;
    (void)route;

    if (config.getErrorsPages().find(std::atoi(code.c_str())) != config.getErrorsPages().end())
    {
        page.open((config.getErrorsPages().find(std::atoi(code.c_str()))->second).c_str());
        if (!page.is_open())
        {
            page.close();
            page.open(("pages/" + code + ".html").c_str());
        }
    }
    else
        page.open(("pages/" + code + ".html").c_str());
    std::string body;
	std::string line;
    
    if (page.good())
    {
		while (getline(page, line))
			body += line;
	}
    std::stringstream ss;
    ss << body.length();
    std::string rep = "HTTP/1.1 " + code + " " + response.getErrorsPages()[std::atoi(code.c_str())] + "\nContent-Type: text/html\nContent-Length: " + ss.str() + "\nServer: webserv\n\n";
    rep += body;
    page.close();
    return (rep);
}

std::string contentLength(Response &response)
{
    if (response.getContentLength() == "0")
        return (response.getTreeLength());
    return (response.getContentLength());
}

std::string getDisplayDate(void)
{
    time_t now = time(0);
	struct tm tstruct;
	char buffer[80];

	tstruct = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%H:%M:%S", &tstruct);
	std::stringstream ss;
	ss << buffer;
    return (ss.str());
}

int WebServ::getServerFd() const
{
	return (this->server_fd);
}

int WebServ::getEpollFd() const
{
	return (this->epoll_fd);
}

ServerConfig WebServ::getConfig()
{
	return (this->config);
}

void WebServ::setEpollFd(int epoll)
{
    this->epoll_fd = epoll;
}

void WebServ::setConfig(ServerConfig &config)
{
    this->config = config;
}

bool WebServ::is_valid_host(const char* host)
{
    struct addrinfo hints;
    struct addrinfo* res;

    std::memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(host, NULL, &hints, &res);
    if (status != 0) {
        std::cerr << "getaddrinfo error: " << gai_strerror(status) << std::endl;
        return false;
    }

    freeaddrinfo(res);
    return true;
}
