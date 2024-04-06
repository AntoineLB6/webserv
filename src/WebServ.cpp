/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServ.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 17:38:07 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/06 14:21:25 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "WebServ.hpp"
#include "CGIHandler.hpp"
#include "Request.hpp"

WebServ::WebServ(struct WebConfig& config, int epoll_fd_to_set) : config(config)
{
    this->epoll_fd = epoll_fd_to_set;
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
    
    const int enable = 1;
    if (setsockopt(this->server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
	{
		std::cerr << "Error Socket Option : " << std::strerror(errno) << std::endl;
		exit(EXIT_FAILURE);
	}

    if (fcntl(this->server_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Error Setting Socket Flags : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
	
    if (!this->config.server_name.empty())
	{
		if (!this->is_valid_host(config.server_name.c_str()) || (config.port < 0 || config.port >= 65535))
		{
			std::cerr << "Error in host & server_name params : " << std::strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	std::memset(&this->address, 0, sizeof(this->address));
	this->address.sin_family = AF_INET;
    if (!this->config.server_name.empty())
	    this->address.sin_addr.s_addr = inet_addr(this->config.server_name.c_str());
    else
		this->address.sin_addr.s_addr = INADDR_ANY;
	this->address.sin_port = htons(config.port);

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
	this->event.events = EPOLLIN | EPOLLOUT;
    this->event.data.fd = this->server_fd;
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, this->server_fd, &this->event) == -1)
    {
        std::cerr << "Error Adding Epoll : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int WebServ::create_client()
{
    int addrlen = sizeof(&address);
    int client_fd;

    if ((client_fd = accept(this->server_fd, (struct sockaddr *)&this->address, (socklen_t *)&addrlen)) < 0) {
        std::cerr << "Error Accepting : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        std::cerr << "Error Setting Socket Flags client : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    struct epoll_event client_event;

    client_event.events = EPOLLIN | EPOLLOUT;
    client_event.data.fd = client_fd;
    
    if (epoll_ctl(this->epoll_fd, EPOLL_CTL_ADD, client_fd, &client_event) < 0)
    {
        std::cerr << "Error Accepting EPOLL : " << std::strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    return client_fd;
}

std::string handleGET(Request &req, struct RouteConfig route)
{
    Response response;
    std::string cgiBody = "";
    std::string rep;
    // location redirect
    // location errorPages
    // location allowedMethods
    // location root
    // location uri
    // location index
    // init un objet location
    if (req.getPath() != "/")
    {
        // check allowed methods => 405
        if (req.getPath().find("cgi-bin") != std::string::npos)
        {
            cgiBody = handleCGI(req, response);
        }
    }
    if (req.getPath().find("cgi-bin") != std::string::npos)
    {
        response.setHeaders(req, 1, cgiBody, route);
    }
    else
        response.setHeaders(req, 0, cgiBody, route);
    if (!cgiBody.empty())
        rep = response.getResponse() + cgiBody;
    else
        rep = response.getResponse();
    return (rep);
}

std::string handlePOST(Request &req, struct RouteConfig route)
{
    Response response;
    std::string cgiBody = "";
    std::string rep;
    
    if (req.getPath() != "/")
    {
        // check allowed methods => 405
        // check max body size => 413
        if (req.getPath().find("cgi-bin") != std::string::npos)
        {
            cgiBody = handleCGI(req, response);
        }
    }
    if (req.getPath().find("cgi-bin") != std::string::npos)
    {
        response.setHeaders(req, 1, cgiBody, route);
    }
    else
        response.setHeaders(req, 0, cgiBody, route);
    if (!cgiBody.empty())
        rep = response.getResponse() + cgiBody;
    else
        rep = response.getResponse();
    if (response.getStatusCode() == "200")
    {
        std::string contentType = req.getContentType();
        contentType = contentType.substr(0, contentType.find(";"));
        std::cout << contentType << std::endl;
        // if (contentType == "application/x-www-form-urlencoded")
        //     rep = handleForm(req);
        // if (contentType == "multipart/form-data")
        //     rep = handleFileUploads(req);
    }
    return (rep);
}

std::string handleDELETE(Request &req, struct RouteConfig route)
{
    Response response;
    (void)req;
    (void)route;

    return (response.getResponse());
}

std::string handleForm(Request &req)
{
    (void)req;
    Response response;

    
    return (NULL);
}

std::string handleFileUploads(Request &req)
{
    (void)req;
    return (NULL);
}

std::string handleCGI(Request &req, Response &response)
{
    std::string body = response.handleCGI(req);
    
    if (body.empty())
    {
        std::cerr << "Error exec CGI" << std::endl;
        return (NULL);
    }
    return (body);
}

std::string getErrorsPages(std::string code)
{
    std::ifstream page;

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
    std::string rep = "HTTP/1.1 501 Internal Server Error\nContent-Type: text/html\nContent-Length: " + ss.str() + "\nServer: webserv\n\n";
    rep += body;
    page.close();
    return (rep);
}

int WebServ::getServerFd() const
{
	return (this->server_fd);
}

int WebServ::getEpollFd() const
{
	return (this->epoll_fd);
}

// long WebServ::getMaxBodySize() const
// {
//     return (this->config.client_max_body_size);
// }

WebConfig WebServ::getConfig() const
{
	return (this->config);
}

void WebServ::setEpollFd(int epoll)
{
    this->epoll_fd = epoll;
}

void WebServ::setConfig(WebConfig &config)
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
