# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nmei <nmei@student.42.fr>                  +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2017/11/27 08:57:48 by nmei              #+#    #+#              #
#    Updated: 2018/02/04 20:30:31 by nmei             ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = libft.a

RM = /bin/rm -f
CFLAGS = -Wall -Wextra -Werror -O2

INCLUDES = -I ./includes/
SRCS_DIR = ./
FILES = ft_memset ft_bzero ft_memcpy ft_memccpy ft_memmove ft_memchr ft_memcmp \
ft_strlen ft_strdup ft_strcpy ft_strncpy ft_strcat ft_strncat ft_strlcat \
ft_strchr ft_strrchr ft_strstr ft_strnstr ft_strcmp ft_strncmp ft_atoi \
ft_isalpha ft_isdigit ft_isalnum ft_isascii ft_isprint ft_toupper ft_tolower \
ft_memalloc ft_memdel ft_strnew ft_strdel ft_strclr ft_striter ft_striteri \
ft_strmap ft_strmapi ft_strequ ft_strnequ ft_strsub ft_strjoin ft_strtrim \
ft_strsplit ft_itoa ft_putchar ft_putstr ft_putendl ft_putnbr ft_putchar_fd \
ft_putstr_fd ft_putendl_fd ft_putnbr_fd ft_lstnew ft_lstdelone ft_lstdel \
ft_lstadd ft_lstiter ft_lstmap ft_memdup ft_lstlen ft_lstrev ft_abs ft_lstlast \
ft_get_next_line ft_num_words ft_splitdel ft_read_all

PRINTF_INC = -I ./ft_printf/includes/
PRINTF_DIR = ./ft_printf/
PF_FILES = ft_printf parse_prespecifiers parse_specifier\
printf_double_utils printf_gen_utils printf_int_utils printf_utf_utils\
double_handlers int_handlers special_handlers hard_double_handlers \
str_char_handlers uint_handlers

CFILES = $(patsubst %, $(SRCS_DIR)%.c, $(FILES))
OFILES = $(patsubst %, %.o, $(FILES))

PF_CFILES = $(patsubst %, $(PRINTF_DIR)%.c, $(PF_FILES))
PF_OFILES = $(patsubst %, $(SRCS_DIR)%.o, $(PF_FILES))

.PHONY: all clean fclean re

all : $(NAME)

$(NAME) : $(OFILES) $(PF_OFILES)
	ar rc $(NAME) $(OFILES) $(PF_OFILES)
	ranlib $(NAME)

$(OFILES) :
	gcc $(CFLAGS) -c $(CFILES) $(INCLUDES)

$(PF_OFILES) :
	gcc $(CFLAGS) -c $(PF_CFILES) $(PRINTF_INC) $(INCLUDES)

clean :
	$(RM) $(OFILES)
	$(RM) $(PF_OFILES)

fclean : clean
	$(RM) $(NAME)

re : fclean all
