/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/10 20:39:27 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <errno.h>             // errno
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <stdbool.h>           // boolean
# include <stdio.h>             // printf, readline     //, strerror, perror
# include <stdlib.h>            // free      // malloc, free, exit, getenv
# include <unistd.h>            // chdir,

# define BUFLEN 256
# define REJECT " \f\n\r\t\v<>|"
# define OPERATORS "<>|"
# define E_PARSE "minishell: parse error"	// >>> or > >
# define E_ENV "bad substitution"	// ${{name}}

enum	e_token_type
{
	word,
	squote,
	dquote,
	pipe_char,
	redir_in,  // <
	redir_out, // >
	append,    // >>
	heredoc    // <<
				// assignment
};

typedef enum
{
	TYPE_TOKEN,
	TYPE_CMD,
	TYPE_REDIR
}	e_node_type;

typedef struct s_token
{
	enum e_token_type	type;
	char				*token;
	size_t				len;
	struct s_token		*next;
}						t_token;

typedef struct s_string
{
	char	*str;
	size_t	i;
	size_t	cap;
}			t_string;

typedef struct s_redir
{
	char				*file;
	enum e_token_type	flag;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	struct s_cmd *next;
}			t_cmd;

typedef union
{
	t_token	*token;
	t_cmd	*cmd;
	t_redir	*redir;
}			u_node;

// lexer.c
t_token					*split_into_tokens(char *s);
void					free_list(t_token *tokens, bool free_content);
t_token					*new_node(void *token);
void					lstadd_back(void **tokens, void *new, void *last, e_node_type type);

// parser.c
t_token					*parse_tokens(t_token *tokens);

//ast.c
t_cmd				*build_ast(t_token *tokens);
void					free_cmds(t_cmd *cmds);

// utils.c
int						ft_isspace(int c);
int						ft_isalnum(int c);
char					*ft_strchr(const char *s, int c);
size_t					ft_strspn(const char *s, const char *accept);
size_t					ft_strcspn(const char *s, const char *reject);
int						ft_strncmp(const char *s1, const char *s2, size_t n);
char					*ft_strndup(const char *s1, size_t len);
void					*ft_memcpy(void *dst, const void *src, size_t n);
size_t					ft_strlen(const char *s);
char					*ft_strjoin(const char *s1, const char *s2);
void					*ft_realloc(void *ptr, size_t old_size, size_t size);

#endif