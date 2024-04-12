/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/13 01:18:19 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/13 01:18:21 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"

AutoIndex::AutoIndex()
{

}

bool AutoIndex::isDirectory(const std::string& path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		return false;
	}
	return S_ISDIR(info.st_mode);
}

bool AutoIndex::isFile(const std::string& path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0) {
		return false;
	}
	return S_ISREG(info.st_mode);
}

std::vector<std::string> AutoIndex::listFiles(const std::string& directory)
{
	std::vector<std::string> files;
	DIR* dir = opendir(directory.c_str());
	if (dir != NULL) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_REG) {
				files.push_back(entry->d_name);
			}
		}
		closedir(dir);
	}
	return files;
}

std::vector<std::string> AutoIndex::listDirectories(const std::string& directory) {
	std::vector<std::string> directories;
	DIR* dir = opendir(directory.c_str());
	if (dir != NULL) {
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL) {
			if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
				directories.push_back(entry->d_name);
			}
		}
		closedir(dir);
	}
	return directories;
}

std::string AutoIndex::generateAutoIndexHTML(std::string &path) {
	std::stringstream html;
	html << "<html><head><title>Index of " << path << "</title></head><body>";
	html << "<h1>Index of " << path << "</h1>";
	html << "<ul>";
	if (path != "/") {
		html << "<li><a href=\"../\">../</a></li>";
	}
	std::vector<std::string> directories = listDirectories(path);
	for (size_t i = 0; i < directories.size(); ++i) {
		html << "<li><a href=\"" << directories[i] << "/\">" << directories[i] << "/</a></li>";
	}
	std::vector<std::string> files = listFiles(path);
	for (size_t i = 0; i < files.size(); ++i) {
		html << "<li><a href=\"" << files[i] << "\">" << files[i] << "</a></li>";
	}
	html << "</ul></body></html>";
	return (html.str());
}
