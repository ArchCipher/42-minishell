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

# include <fcntl.h>				// open
# include <errno.h>             // errno
# include <limits.h>			// LONG_MAX (ft_atoi)
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <stdbool.h>           // boolean
# include <stdio.h>             // printf, readline     //, strerror, perror
# include <stdlib.h>            // malloc, free, exit, getenv
# include <sys/stat.h>			// stat
# include <sys/wait.h>			// waitpid
# include <unistd.h>            // read, write, pipe, fork, execve, access, chdir, dup, dup2

# define REJECT " \f\n\r\t\v<>|"
# define OPERATORS "<>|"
# define E_PARSE "minishell: syntax error"	// >>> or > > (bash: syntax error near unexpected token `>')
# define E_ENV "minishell: bad substitution"	// ${{name}}
// # define E_PATH "ls: bin: No such file or directory" // ls bin

typedef enum
{
	word,
	squote,
	dquote,
	pipe_char,
	redir_in,  // <
	redir_out, // >
	append,    // >>
	heredoc    // <<
	// and_op	// &&
	// or_op	// ||
	// parenthesis	// ()
}	e_token_type;

// for wildcard (*) check glob()

typedef enum
{
	TYPE_TOKEN,
	TYPE_CMD,
	TYPE_REDIR
}	e_node_type;

typedef enum
{
	ECHO,
	CD,
	PWD,
	EXPORT,
	UNSET,
	ENV,
	EXIT
}	e_built_in;

typedef struct s_string
{
	char	*str;
	size_t	i;
	size_t	cap;
}			t_string;

typedef struct s_token
{
	e_token_type		type;
	char				*token;
	size_t				len;
	struct s_token		*next;
}						t_token;

typedef struct s_redir
{
	char				*file;
	e_token_type		flag;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	pid_t			pid;
	int				built_in;
	struct s_cmd	*next;
}			t_cmd;

typedef struct s_list
{
	void *head;
	void *tail;
	void *new;
}		t_list;

typedef struct s_exec
{
	char	**args;
	int		write_fd;
	int		read_fd;
}	t_exec;

// lexer.c
t_token					*tokenise_input(char *s);
void					lstadd_back(void **tokens, void *new, void *last, e_node_type type);

// parser.c
t_token					*parse_tokens(t_token *tokens);

//ast.c
t_cmd					*build_ast(t_token *tokens);

// error.c
void					free_tokens(t_token *tokens, bool free_content);
void					free_cmds(t_cmd *cmds);
t_cmd					*error_free(t_cmd *cmds, t_token *tokens);

// execute.c
int						exec_cmds(t_cmd *cmds, char **envp, int *status);

// builtin.c
void	exec_echo(char **args);
void	exec_cd(const char *path);
void	exec_pwd(void);
void    exec_exit(char *s);

// utils.c
int						ft_isspace(int c);
int						ft_isalnum(int c);
char					*ft_strchr(const char *s, int c);
size_t					ft_strspn(const char *s, const char *accept);
size_t					ft_strcspn(const char *s, const char *reject);
int						ft_strcmp(const char *s1, const char *s2);
char					*ft_strdup(const char *s1);
char					*ft_strndup(const char *s1, size_t len);
void					*ft_memcpy(void *dst, const void *src, size_t n);
size_t					ft_strlen(const char *s);
char					*ft_strjoin(const char *s1, const char *s2);
void					*ft_realloc(void *ptr, size_t old_size, size_t size);
char					*ft_strtok_r(char *s, const char *sep, char **p);
char					*ft_strjoin3(const char *s1, const char *s2, const char *s3);
int						ft_atoi(const char *str);
char					*ft_itoa(int n);

#endif