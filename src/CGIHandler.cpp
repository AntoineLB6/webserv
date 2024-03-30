/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:06:41 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/30 23:42:27 by lmoheyma         ###   ########.fr       */
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

void CGIHandler::setCgiEnv(Request &req)
{
	this->_envMap["CONTENT_LENGTH"] = req.getContentLength();
	this->_envMap["CONTENT_TYPE"] = req.getContentType();
	this->_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_envMap["SCRIPT_FILENAME"] ="cgi-bin/test.php";
	this->_envMap["SERVER_PORT"] = req.getPort();
	this->_envMap["REQUEST_METHOD"] = req.getHeaders()["Method"];
	this->_envMap["SERVER_PROTOCOL"] = req.getHeaders()["Version"];
	this->_envMap["HTTP_ACCEPT"] = req.getHeaders()["Accept"];
	this->_envMap["HTTP_ACCEPT_LANGUAGE"] = req.getHeaders()["Accept-Language"];
	this->_envMap["HTTP_USER_AGENT"] = req.getHeaders()["User-Agent"];
	this->_envMap["HTTP_REFERER"] = req.getHeaders()["Referer"];
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
	_argv = (char **)calloc(sizeof(char *), 2);
	if (!_argv)
		return ;
	// _argv[0] = (char *)strdup(req.getPath().c_str());
	// _argv[1] = (char *)strdup(req.getPath().c_str());
	_argv[0] = (char *)strdup("/usr/bin/php-cgi");
	_argv[1] = NULL;
	// for (int i = 0; i < 3; i++)
		// std::cout << _argv[i] << std::endl;
	printEnv();
}

std::string CGIHandler::execute(int pip[2], Request &req)
{
	std::string responseBody;
	int fdout[2];
	int fdin[2];
	
	pipe(fdin);
	pipe(fdout);
	(void)pip;
	
	pid_t pid = fork();
	if (pid == 0)
	{
		dup2(fdout[1], STDOUT_FILENO);
		close(fdout[0]);
		close(fdout[1]);
		dup2(fdin[0], STDIN_FILENO);
		close(fdin[1]);
		close(fdin[0]);
		execve(_argv[0], _argv, _env);
	}
	else
	{
		close(fdin[0]);
		write(fdin[1], req.getHeaders()["Body"].c_str(), req.getHeaders()["Body"].size());
		close(fdin[1]);
		close(fdout[1]);
		int status;
		waitpid(pid, &status, 0);
		char buffer[1024];
		int ret = 0;
		do {
			memset(buffer, 0, 1024);
			ret = read(fdout[0], buffer, 1024);
			responseBody.append(buffer, ret);
		} while (ret > 0);
		close(fdout[0]);
	}
	return (responseBody);
}

void CGIHandler::printEnv(void)
{
	int i = 0;
	
	while (_env[i])
	{
		std::cout << _env[i++] << std::endl;
	}
}