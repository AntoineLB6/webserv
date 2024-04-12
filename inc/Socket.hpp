/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmoheyma <lmoheyma@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/10 15:38:16 by lmoheyma          #+#    #+#             */
/*   Updated: 2024/04/10 16:30:26 by lmoheyma         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <time.h>
#include <iostream>
#include "main.hpp"

class Socket
{
	private:
		const int fd;
		const int server_fd;
		std::string buffer;
		clock_t time;

	public:
		Socket();
		Socket(int fd, int server_fd);
		~Socket();
		Socket(const Socket &cpySocket);
		Socket &operator=(const Socket &cpySocket);
		int getFd() const;
		int getServerFd() const;
		int getBufferSize() const;
		std::string getBuffer() const;
		clock_t getTime() const;
		void addToBuffer(char *bufferToAdd);
};
