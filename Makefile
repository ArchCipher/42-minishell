#				Library and Project names
NAME			= minishell

#				Headers
INC				= -I.

#				Detect OS and set readline paths
UNAME_S			= $(shell uname -s)
READLINE_DIR	= $(shell brew --prefix readline 2>/dev/null || echo "")
READLINE_INC	= $(if $(READLINE_DIR),-I$(READLINE_DIR)/include,)
READLINE_LIB	= $(if $(READLINE_DIR),-L$(READLINE_DIR)/lib,)

#				Sources & Objects
MSRCS			= minishell lexer parser ast free heredoc execute builtin signal utils

SRCS			= $(addsuffix .c, $(MSRCS))
OBJS			= $(SRCS:.c=.o)

# .DEFAULT_GOAL := all

#				Compiler and Flags
CC				= cc
FLAGS			= -Wall -Werror -Wextra
SFLAGS			= -fsanitize=address

CFLAGS			= $(FLAGS) $(SFLAGS) $(READLINE_INC) $(READLINE_LIB) -lreadline

%.o: %.c
	$(CC) $(FLAGS) $(INC) $(READLINE_INC) -g -c $< -o $@

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -g $(OBJS) -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re check_readline
