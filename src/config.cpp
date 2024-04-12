/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   config.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:18:06 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 01:18:08 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/main.hpp"

void getConfig(std::string path, std::vector<ServerConfig> &server_configs)
{
	std::ifstream file(path.c_str());
	if (!file)
	{
		std::cerr << "Can't open file : " << path << std::endl;
        exit(EXIT_FAILURE);
	}
	std::string line;
	std::string currentConfig;
    ServerConfig config;
    int trigger = 0;
	while (std::getline(file, line))
	{
        line = trim(line);
		size_t pos = line.find('#');
		if (pos != std::string::npos)
			line = line.substr(0, pos);
		if (line.empty())
			continue ;
        
		size_t openBracePos = line.find('{');
		if (openBracePos != std::string::npos)
		{
			currentConfig = line.substr(0, openBracePos);
            currentConfig = trim(currentConfig);
            if (trigger > 1 && currentConfig != "server")
            {
                std::cerr << "Error in Config File : " << line << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (currentConfig == "server")
            {
                ServerConfig new_config;
                config = new_config;
                config.clear();
            }
            trigger++;
		}
		else if (line.find('}') != std::string::npos)
		{
            if (trigger < 1)
            {
                std::cerr << "Error in Config File : " << "nothing to close" << std::endl;
                exit(EXIT_FAILURE);
            }
            trigger--;
            if (trigger == 1)
            {
                currentConfig = "server";
            }
            else
            {
                currentConfig = "end";
                if (config.isRouteEmpty())
                {
                    RouteConfig route;
                    config.insertRoute("/", route);
                }
                for (std::map<std::string, RouteConfig>::iterator it = config.getRoutes().begin(); it != config.getRoutes().end(); it++)
                {
                    RouteConfig& route = it->second;
                    if (route.isMethodsEmpty())
                        route.addOneMethod("GET");
                }
                ServerConfig push_config(config);
                server_configs.push_back(push_config);
            }
			continue ;
		}
		else
		{
            pos = line.find(';');
		    if (pos != std::string::npos)
			    line = line.substr(0, pos);
			std::vector<std::string> tokens = split(line, ' ');
			if (tokens.size() == 2 && tokens[0] != "limit_except" && tokens[0] != "return")
            {
                put_setting(tokens[0], tokens[1], currentConfig, config);
            }
            else if (tokens.size() == 3 && tokens[0] == "return")
            {
                put_setting_return(tokens, currentConfig, config);
            }
            else if (tokens.size() == 3 && tokens[0] == "error_page")
            {
                put_setting_error_page(tokens, currentConfig, config);
            }
            else if (tokens.size() >= 2 && tokens[0] == "limit_except")
            {
                put_setting_http(tokens, currentConfig, config);
            }
            else
            {
                std::cerr << "Error in Config File : " << line << std::endl;
                exit(EXIT_FAILURE);
            }
		}
	}
    if (trigger)
    {
        std::cerr << "Error in Config File : " << "not closed" << std::endl;
        exit(EXIT_FAILURE);
    }
	file.close();
}


std::string trim(const std::string& str)
{
    size_t first = str.find_first_not_of(' ');
    if (std::string::npos == first) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::vector<std::string> split(const std::string &s, char delimiter)
{
	std::vector<std::string> tokens;
	std::string token;
	std::istringstream tokenStream(s);
	while (std::getline(tokenStream, token, delimiter))
	{
		if (!token.empty())
		{
			tokens.push_back(token);
		}
	}
	return (tokens);
}

void put_setting(std::string key, std::string value, std::string currentConfig, ServerConfig &config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "server")
    {
        if (key == "listen")
            config.setPort(std::atoi(value.c_str()));
        else if (key == "server_name")
            config.setServerName(value);
        else
        {
            std::cerr << "Error in Config File : " << key << std::endl;
            exit(EXIT_FAILURE);
        }
        
    }
    else if (config_words[0] == "location")
    {
        RouteConfig &route = config.getOrSetRoute(config_words[1]);
        if (key == "root")
            route.setRoot(value);
        else if (key == "autoindex" && (value == "on" || value == "off"))
            value == "on" ? route.setAutoindex(true) : route.setAutoindex(false);
        else if (key == "default")
            route.setDefault(value);
        else if (key == "client_body_temp_path")
            route.setClientBodyTempPath(value);
        else if (key == "client_max_body_size")
            route.setClientMaxBodySize(std::atol(value.c_str()));
        else
        {
            std::cerr << "Error in Config File : " << key << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_return(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "location")
    {
        RouteConfig &route = config.getOrSetRoute(config_words[1]);
        if (tokens[0] == "return")
        {
            route.setOneReturnCode(std::atoi(tokens[1].c_str()), tokens[2]);
        }
        else
        {
            std::cerr << "Error in Config File : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_http(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "location")
    {
        RouteConfig &route = config.getOrSetRoute(config_words[1]);
        if (tokens[0] == "limit_except")
        {
            for (size_t i = 1; i < tokens.size(); i++)
            {
                route.addOneMethod(tokens[i]);
            }
        }
        else
        {
            std::cerr << "Error in Config File : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_error_page(std::vector<std::string> tokens, std::string currentConfig, ServerConfig &config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] != "location")
    {
        if (tokens[0] == "error_page")
        {
            config.setOneErrorPage(std::atoi(tokens[1].c_str()), tokens[2]);
        }
        else
        {
            std::cerr << "Error in Config File : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}