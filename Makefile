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

#				Compiler and Flags
CC				= cc
FLAGS			= -Wall -Werror -Wextra
SFLAGS			= -fsanitize=address

CFLAGS			= $(FLAGS) $(SFLAGS) $(READLINE_INC) $(READLINE_LIB) -lreadline

%.o: %.c
	$(CC) $(FLAGS) $(INC) $(READLINE_INC) -g -c $< -o $@

.PHONY: check_readline

check_readline:
	@if [ "$(UNAME_S)" = "Darwin" ]; then \
		if [ -z "$(READLINE_DIR)" ] || [ ! -d "$(READLINE_DIR)" ]; then \
			echo "Error: GNU readline not found."; \
			echo "Please install it with: brew install readline"; \
			exit 1; \
		fi; \
	fi

all: check_readline $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -g $^ -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all bonus

.PHONY: all clean fclean re bonus
