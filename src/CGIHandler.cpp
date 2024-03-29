/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:06:41 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/29 23:26:10 by lmoheyma         ###   ########.fr       */
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
	this->_envMap["CONTENT_LENGTH"] =  req.getHeaders()["Body"].length();
	this->_envMap["CONTENT_TYPE"] = req.getContentType();
	this->_envMap["GATEWAY_INTERFACE"] = "CGI/1.1";
	this->_envMap["SCRIPT_NAME"] = req.getPath();
	this->_envMap["SERVER_PORT"] = req.getPort();
	this->_envMap["REQUEST_METHOD"] = req.getHeaders()["Method"];
	this->_envMap["SERVER_PROTOCOL"] = req.getHeaders()["Version"];
	this->_envMap["HTTP_ACCEPT"] = req.getHeaders()["Accept"];
	this->_envMap["HTTP_ACCEPT_LANGUAGE"] = req.getHeaders()["Accept-Language"];
	this->_envMap["HTTP_USER_AGENT"] = req.getHeaders()["User-Agent"];
	this->_envMap["HTTP_REFERER"] = req.getHeaders()["Referer"];
	_env = (char **)calloc(sizeof(char *), _envMap.size() + 1);
	if (!_env)
		return ;
	std::map<std::string, std::string>::iterator it;
	int i = 0;
	for (it = _envMap.begin(); it != _envMap.end(); it++)
	{
		std::string tmp = it->first + "=" + it->second;
		_env[i] = strdup(tmp.c_str());
	}
	_argv = (char **)calloc(sizeof(char *), 3);
	_argv[0] = (char *)strdup(req.getPath().c_str());
	_argv[1] = (char *)strdup(req.getPath().c_str());
	_argv[2] = NULL;
}

int CGIHandler::execute(std::string file)
{
	if (pipe(pipein) == -1)
		return (1);
	if (pipe(pipeout) == -1)
		return (1);
	int cgiPid = fork();
	if (cgiPid < 0)
		return (0);
	if (!cgiPid)
	{
		if (dup2(pipein[0], STDIN_FILENO))
			return (1);
		if (dup2(pipeout[1], STDOUT_FILENO))
			return (1);
		close(pipein[0]);
		close(pipein[1]);
		close(pipeout[0]);
		close(pipeout[1]);
		execve(_argv[0], _argv, _env);
		std::cout << "Execve failed" << std::endl;
		exit(EXIT_FAILURE);
	}
}