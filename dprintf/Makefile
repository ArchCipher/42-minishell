#add header for all files

#			Library and Project names
NAME		= libftdprintf.a

#			Compiler and Flags
CC			= cc
FLAGS		= -Wall -Werror -Wextra

#			Includes
HEADER		= ft_dprintf.h
INC			= -I.

#			Sources & Objects
MSRCS 		= \
	   		dprintf parser print_types print_int printf_utils
SRCS		= $(addprefix ft_, $(addsuffix .c, $(MSRCS)))
OBJS		= $(SRCS:.c=.o)

%.o: %.c $(HEADER)
	$(CC) $(FLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	ar rcs $@ $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean