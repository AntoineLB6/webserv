/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:06:41 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/15 13:41:18 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"

CGIHandler::CGIHandler()
{
	
}

CGIHandler::CGIHandler(const CGIHandler &other)
{
	(void)other;	
}

CGIHandler& CGIHandler::operator=(const CGIHandler &other)
{
	(void)other;
	return (*this);
}

CGIHandler::~CGIHandler()
{

}

void CGIHandler::setCgiPath(std::string path)
{
	this->_cgiPath = path;
}

void CGIHandler::setCgiEnv(Request &req, std::string path, ServerConfig config)
{
	this->_envMap["CONTENT_LENGTH"] = req.getContentLength();
	this->_envMap["CONTENT_TYPE"] = req.getContentType(config);
	this->_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_envMap["QUERY_STRING"] = req.getBody();
	this->_envMap["SCRIPT_FILENAME"] = path;
	this->_envMap["SERVER_PORT"] = req.getPort();
	this->_envMap["REQUEST_METHOD"] = req.getHeaders()["Method"];
	this->_envMap["SERVER_PROTOCOL"] = req.getHeaders()["Version"];
	this->_envMap["HTTP_ACCEPT"] = req.getHeaders()["Accept"];
	this->_envMap["HTTP_ACCEPT_LANGUAGE"] = req.getHeaders()["Accept-Language"];
	this->_envMap["HTTP_USER_AGENT"] = req.getHeaders()["User-Agent"];
	this->_envMap["HTTP_REFERER"] = req.getHeaders()["Referer"];
	if (!req.getHeaders()["Cookie"].empty())
		this->_envMap["HTTP_COOKIE"] = req.getHeaders()["Cookie"];
	this->_envMap["REDIRECT_STATUS"] = "200";
	_env = (char **)calloc(sizeof(char *), _envMap.size() + 1);
	if (!_env)
		return ;
	std::map<std::string, std::string>::iterator it;
	int i = 0;
	for (it = _envMap.begin(); it != _envMap.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		_env[i] = strdup(tmp.c_str());
		i++;
	}
	// printEnv();
}

std::string CGIHandler::execute(Request &req, std::string path)
{
	std::string responseBody;
	int fdout[2];
	int fdin[2];
	char* const *temp = NULL;
	int status;
	
	if (pipe(fdin) == -1)
		return (NULL);
	if (pipe(fdout) == -1)
		return (NULL);
	pid_t pid = fork();
	if (pid < 0)
		return (NULL);
	if (pid == 0)
	{
		if (dup2(fdout[1], STDOUT_FILENO) == -1)
			return (NULL);
		close(fdout[0]);
		close(fdout[1]);
		if (dup2(fdin[0], STDIN_FILENO) == -1)
			return (NULL);
		close(fdin[1]);
		close(fdin[0]);
		execve(path.c_str(), temp, _env);
		std::cerr << BOLDRED;
		perror("Execve: ");
		std::cerr << RESET;
		exit(EXIT_FAILURE);
	}
	else
	{	
		close(fdin[0]);
		write(fdin[1], req.getHeaders()["Body"].c_str(), req.getHeaders()["Body"].size());
		close(fdin[1]);
		close(fdout[1]);
		waitpid(pid, &status, 0);
		char buffer[1024];
		int bytes = 0;
		do {
			memset(buffer, 0, 1024);
			bytes = read(fdout[0], buffer, 1024);
			responseBody.append(buffer, bytes);
		} while (bytes > 0);
		close(fdout[0]);
	}
	return (responseBody);
}

void CGIHandler::printEnv(void)
{
	int i = 0;
	
	while (_env[i])
		std::cout << _env[i++] << std::endl;
}