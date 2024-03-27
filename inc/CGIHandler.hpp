/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/27 15:07:11 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/03/27 17:40:32 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

class CGIHandler
{
	public:
		CGIHandler();
		CGIHandler(const CGIHandler &other);
		CGIHandler& operator=(const CGIHandler &other);
		~CGIHandler();
		void execute(std::string file);
};