/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:07:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/28 16:47:46 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>

class CGIHandler
{
	private:
		char **_env;
	public:
		CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler& operator=(const CGIHandler &other);
		~CGIHandler();
		int execute(std::string file);
		void setEnv();
};