#				Library and Project names
NAME			= minishell
LIBFT_LIB		= libft

#				Headers
INC				= -I.

#				Detect OS and set readline paths
UNAME_S			= $(shell uname -s)
READLINE_DIR	= $(shell brew --prefix readline 2>/dev/null || echo "")
READLINE_INC	= $(if $(READLINE_DIR),-I$(READLINE_DIR)/include,)
READLINE_LIB	= $(if $(READLINE_DIR),-L$(READLINE_DIR)/lib,)

#				Sources & Objects
MSRCS			= minishell signal \
				lexer \
				parser dollar \
				ast heredoc \
				execute exec_child exec_utils \
				builtin export export_no_args qsort env unset exit \
				free

SRCS			= $(addsuffix .c, $(MSRCS))
OBJS			= $(SRCS:.c=.o)

# .DEFAULT_GOAL := all

#				Compiler and Flags
CC				= cc
FLAGS			= -Wall -Werror -Wextra
SFLAGS			= -fsanitize=address

CFLAGS			= $(FLAGS) $(SFLAGS) $(READLINE_INC) $(READLINE_LIB) -lreadline

#			Library
FT_LIBFT	= -L$(LIBFT_LIB) -lft

%.o: %.c
	$(CC) $(FLAGS) $(INC) $(READLINE_INC) -g -c $< -o $@

all: lib $(NAME)

lib:
	@make -C $(LIBFT_LIB)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -g $(OBJS) $(FT_LIBFT) -o $@

clean:
	make -C $(LIBFT_LIB) clean
	rm -f $(OBJS)

fclean: clean
	make -C $(LIBFT_LIB) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re lib
