# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2018/01/29 22:01:53 by nmei              #+#    #+#              #
#    Updated: 2018/02/08 23:16:44 by nmei             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = ft_ssl
RM = /bin/rm -f
SRCS_DIR = ./
INCLUDES = ./includes/
CFLAGS = -Wall -Wextra -Werror

MAIN_FILES = main
MAIN_CFILES = $(patsubst %, $(SRCS_DIR)%.c, $(MAIN_FILES))
MAIN_OFILES = $(patsubst %, $(SRCS_DIR)%.o, $(MAIN_FILES))

B64_DIR = ./base64/
B64_FILES = base64_handler base64_encode base64_decode
B64_CFILES = $(patsubst %, $(B64_DIR)%.c, $(B64_FILES))
B64_OFILES = $(patsubst %, $(SRCS_DIR)%.o, $(B64_FILES))

DES_DIR = ./des/
DES_FILES = des_handler des_key des_utils des_algorithm des
DES_CFILES = $(patsubst %, $(DES_DIR)%.c, $(DES_FILES))
DES_OFILES = $(patsubst %, $(SRCS_DIR)%.o, $(DES_FILES))

CFILES = $(MAIN_CFILES) $(B64_CFILES) $(DES_CFILES)
OFILES = $(MAIN_OFILES) $(B64_OFILES) $(DES_OFILES)

#libft
LFT = ./libft/
LFT_LIB = $(addprefix $(LFT), ft.a)
LFT_INC = -I $(LFT)includes/
LFT_LINK = -L $(LFT) -l ft

.PHONY: all clean fclean re

all: $(LFT_LIB) $(NAME)

$(OFILES):
	gcc $(CFLAGS) -c -I$(INCLUDES) $(LFT_INC) $(CFILES)

$(LFT_LIB):
	make -C $(LFT)

$(NAME): $(OFILES)
	gcc $(CFLAGS) $(OFILES) $(LFT_LINK) -o $(NAME)

clean:
	make -C $(LFT) clean
	$(RM) $(OFILES)

fclean: clean
	make -C $(LFT) fclean
	$(RM) $(NAME)

re: fclean all