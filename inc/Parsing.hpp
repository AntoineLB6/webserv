/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 15:27:28 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/25 18:25:23 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <cstring>

class Parsing
{
	private:
		std::string pagePath;
	public:
		Parsing();
		Parsing(const Parsing &other);
		~Parsing();
		Parsing& operator=(const Parsing &other);
		void parseRequest(std::string request);
		bool tryOpenPage(std::string pageName);
		std::string getPagePath(void) const ;
};