#pragma once

#include "main.hpp"

class AutoIndex
{
	public:

	AutoIndex();
	bool isDirectory(const std::string& path);
	bool isFile(const std::string& path);
	std::vector<std::string> listFiles(const std::string& directory);
	std::vector<std::string> listDirectories(const std::string& directory);
	std::string generateAutoIndexHTML(std::string &path);
};