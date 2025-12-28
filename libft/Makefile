# 			Library Name
NAME		= libft.a

#			Compiler & Flags
CC			= cc
FLAGS		= -Wall -Werror -Wextra

#			Headers
HEADER		= libft.h

#			Sources
MSRCS		= \
			isalnum isalpha isdigit isspace memchr\
			memcpy realloc strchr strcmp strdup\
			strlen strncmp strndup strrchr strspn \
			strtok_r
ASRCS		= \
			strjoin strjoin3

SRCS		= $(MSRCS) $(ASRCS)
MAN_SRCS	= $(addprefix ft_, $(addsuffix .c, $(SRCS)))

#			Objects
MAN_OBJS	= $(MAN_SRCS:.c=.o)

#			Rules
%.o: %.c
			$(CC) $(FLAGS) -c $< -o $@

all:		$(NAME)

$(NAME):	$(MAN_OBJS)
			ar rcs $@ $^

clean:
			rm -f $(MAN_OBJS)

fclean:		clean
			rm -f $(NAME) 

re:			fclean all

.PHONY:		all clean fclean re

#			$@	target name (left side of :)
#			$<	first prerequisite (right side of :)
#			$^	all prerequisites (right side of : (space separated))
#			$?	newer prerequisites (updated ones)
