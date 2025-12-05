#				Library and Project names
NAME			= minishell

#				Headers
INC				= -I.

#				Sources & Objects
MSRCS			= minishell lexer utils

SRCS			= $(addsuffix .c, $(MSRCS))
OBJS			= $(SRCS:.c=.o)

#				Compiler and Flags
CC				= cc
FLAGS			= -Wall -Werror -Wextra
SFLAGS			= -fsanitize=address

CFLAGS			= $(FLAGS) $(SFLAGS) -lreadline

%.o: %.c
	$(CC) $(FLAGS) $(INC) -g -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all bonus

.PHONY: all clean fclean re bonus
