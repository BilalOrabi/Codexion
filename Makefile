# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: borabi <borabi@student.42.fr>              +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/09/19 20:04:00 by borabi            #+#    #+#              #
#    Updated: 2026/09/19 20:04:00 by borabi           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Wextra -Werror -pthread

SRCS		= time.c \
			  heap_utils.c \
			  heap_ops.c

OBJS		= $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)

%.o: %.c codexion.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
