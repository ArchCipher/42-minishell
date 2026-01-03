#				Readline lib
UNAME_S			= $(shell uname -s)

ifeq ($(UNAME_S),Linux)
	READLINE_INC = -I/usr/include/readline
	READLINE_LIB = -lreadline
endif

ifeq ($(UNAME_S),Darwin)
	READLINE_DIR	= $(shell brew --prefix readline 2>/dev/null || echo "")
	READLINE_INC	= $(if $(READLINE_DIR),-I$(READLINE_DIR)/include,)
	READLINE_LIB	= $(if $(READLINE_DIR),-L$(READLINE_DIR)/lib,) -lreadline
endif

#				Library and Project names
NAME			= minishell
#				Library
DIR_LIBFT		= libft
DIR_DPRINTF		= dprintf
LIBFT_A			= $(DIR_LIBFT)/libft.a
DPRINTF_A		= $(DIR_DPRINTF)/libftdprintf.a
FT_LIBFT		= -L$(DIR_LIBFT) -lft
FT_DPRINTF		= -L$(DIR_DPRINTF) -lftdprintf

#				Includes
HEADER			= minishell.h
INC				= -I. \
				-I$(DIR_LIBFT) \
				-I$(DIR_DPRINTF) \
				$(READLINE_INC)

#				Sources & Objects
MSRCS			= minishell \
				lexer parser parser_expand \
				token_checks token_utils \
				ast ast_build \
				var_expand var_parse \
				exec exec_child path redirs heredoc \
				builtin cd export unset exit env_utils qsort \
				signal lst_utils

SRCS			= $(addprefix src/, $(addsuffix .c, $(MSRCS)))
OBJS			= $(SRCS:.c=.o)

#				Compiler and Flags
CC				= cc
FLAGS			= -Wall -Werror -Wextra
SFLAGS			= -fsanitize=address

CFLAGS			= $(FLAGS) $(SFLAGS) -g
LDFLAGS			= $(READLINE_LIB) $(FT_DPRINTF) $(FT_LIBFT) 

%.o: %.c $(HEADER)
	$(CC) $(CFLAGS) $(INC) -c $< -o $@

all: $(NAME)

$(LIBFT_A):
	@echo "--Building libft.a--"
	@$(MAKE) -s -C $(DIR_LIBFT)

$(DPRINTF_A):
	@echo "--Building libftdprintf.a--"
	@$(MAKE) -s -C $(DIR_DPRINTF)

$(NAME): $(DPRINTF_A) $(LIBFT_A) $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@

clean:
	@$(MAKE) -s -C $(DIR_LIBFT) clean
	@$(MAKE) -s -C $(DIR_DPRINTF) clean
	rm -f $(OBJS)

fclean: clean
	@$(MAKE) -s -C $(DIR_LIBFT) fclean
	@$(MAKE) -s -C $(DIR_DPRINTF) fclean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re
