# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: aleite-b <aleite-b@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/02/21 14:25:46 by aleite-b          #+#    #+#              #
#    Updated: 2024/03/21 10:38:18 by aleite-b         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = c++
FLAGS = -Wall -Wextra -Werror -std=c++98
RM = rm -f

SOURCES = webserv.cpp

OBJECTS = $(SOURCES:.cpp=.o)

.cpp.o:
	$(CC) $(FLAGS) -I. -c $< -o $@

$(NAME): $(OBJECTS)
	$(CC) $(FLAGS) $(OBJECTS) -o $(NAME)

all: $(NAME)

re: clean all

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

.PHONY: all clean re fclean