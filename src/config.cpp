#include "../inc/main.hpp"

std::vector<WebConfig> getConfig(std::string path)
{
	std::ifstream file(path.c_str());
	if (!file)
	{
		std::cerr << "Can't open file : " << path << std::endl;
        exit(EXIT_FAILURE);
	}
	std::string line;
	std::string currentConfig;
    std::vector<WebConfig> server_configs;
    struct WebConfig config;
    int trigger = 0;
    config = defaultConfig(config);
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
            if (trigger > 1 && currentConfig != "server")
            {
                std::cerr << "Error in Config File : " << line << std::endl;
                exit(EXIT_FAILURE);
            }
            else if (currentConfig == "server")
            {
                struct WebConfig new_config;
                new_config = defaultConfig(new_config);
                config = new_config;
            }
            trigger++;
			std::cout << "Configuration: " << currentConfig << std::endl;
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
                if (config.routes.empty())
                {
                    struct RouteConfig *route = new RouteConfig;
                    defaultRoute(route);
                    config.routes.insert(std::make_pair("/", *route));
                }
                for (std::map<std::string, RouteConfig>::iterator it = config.routes.begin(); it != config.routes.end(); it++)
                {
                    RouteConfig& route = it->second;
                    if (route.limit_except_accepted.empty())
                        route.limit_except_accepted.push_back("GET");
                }
                server_configs.push_back(config);
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
				std::cout << "Argument: " << tokens[0] << ", Valeur: " << tokens[1] << std::endl;
                put_setting(tokens[0], tokens[1], currentConfig, &config);
            }
            else if (tokens.size() == 3 && tokens[0] == "return")
            {
                put_setting_return(tokens, currentConfig, &config);
            }
            else if (tokens.size() == 3 && tokens[0] == "error_page")
            {
                put_setting_error_page(tokens, currentConfig, &config);
            }
            else if (tokens.size() >= 2 && tokens[0] == "limit_except")
            {
                std::cout << "==" << currentConfig << std::endl;
                put_setting_http(tokens, currentConfig, &config);
            }
            else
            {
                std::cerr << "Error in Config File1 : " << line << std::endl;
                exit(EXIT_FAILURE);
            }
		}
	}
    if (trigger)
    {
        std::cerr << "Error in Config File2 : " << "not closed" << std::endl;
        exit(EXIT_FAILURE);
    }
	file.close();
	return server_configs;
}

void defaultRoute(struct RouteConfig *route_config)
{
    route_config->root = "./pages";
    route_config->autoindex = true;
    route_config->default_page = "index.html";
    route_config->client_body_temp_path = "/var/www/uploads/";
    route_config->client_max_body_size = 1024;
}

struct WebConfig defaultConfig(struct WebConfig config)
{
    config.port = 8080;
    config.server_name = "";
    return (config);
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

void put_setting(std::string key, std::string value, std::string currentConfig, struct WebConfig *config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "server")
    {
        if (key == "listen")
            config->port = std::atoi(value.c_str());
        else if (key == "server_name")
            config->server_name = value;
        else
        {
            std::cerr << "Error in Config File3 : " << key << std::endl;
            exit(EXIT_FAILURE);
        }
        
    }
    else if (config_words[0] == "location")
    {
        struct RouteConfig *route = NULL;
        if (config->routes.find(config_words[1]) != config->routes.end())
            route = &config->routes.find(config_words[1])->second;
        else
        {
            route = new RouteConfig;
            defaultRoute(route);
            config->routes.insert(std::make_pair(config_words[1], *route));
        }
        
        if (key == "limit_except")
            config->port = std::atoi(value.c_str());
        else if (key == "return")
            config->server_name = value;
        else if (key == "root")
            route->root = value;
        else if (key == "autoindex")
            value == "on" ? route->autoindex = true : route->autoindex = false;
        else if (key == "default")
            route->default_page = value;
        else if (key == "client_body_temp_path")
            route->client_body_temp_path = value;
        else if (key == "client_max_body_size")
            route->client_max_body_size = std::atol(value.c_str());
        else
        {
            std::cerr << "Error in Config File4 : " << key << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File5 : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_return(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "location")
    {
        struct RouteConfig *route = NULL;
        if (config->routes.find(config_words[1]) != config->routes.end())
            route = &config->routes.find(config_words[1])->second;
        else
        {
            route = new RouteConfig;
            defaultRoute(route);
            config->routes.insert(std::make_pair(config_words[1], *route));
        }
        
        if (tokens[0] == "return")
        {
            route->return_codes[std::atoi(tokens[1].c_str())] = tokens[2];
        }
        else
        {
            std::cerr << "Error in Config File6 : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File7 : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_http(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] == "location")
    {
        struct RouteConfig *route = NULL;
        if (config->routes.find(config_words[1]) == config->routes.end())
        {
            route = new RouteConfig;
            defaultRoute(route);
            // config->routes[config_words[1]] = *route;
            config->routes.insert(std::make_pair(config_words[1], *route));
        }
        route = &config->routes.find(config_words[1])->second;
        
        if (tokens[0] == "limit_except")
        {
            for (size_t i = 1; i < tokens.size(); i++)
            {
                route->limit_except_accepted.push_back(tokens[i]);
            }
        }
        else
        {
            std::cerr << "Error in Config File8 : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File9 : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}

void put_setting_error_page(std::vector<std::string> tokens, std::string currentConfig, struct WebConfig *config)
{
    std::vector<std::string> config_words = split(currentConfig, ' ');
    if (config_words[0] != "location")
    {
        if (tokens[0] == "error_page")
        {
            config->errors_pages.insert(std::make_pair(std::atoi(tokens[1].c_str()), tokens[2]));
        }
        else
        {
            std::cerr << "Error in Config File10 : " << tokens[0] << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Error in Config File11 : " << currentConfig << std::endl;
        exit(EXIT_FAILURE);
    }
}