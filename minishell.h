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

# include "libft/libft.h"
# include <fcntl.h>				// open
# include <errno.h>             // errno
# include <limits.h>			// LONG_MAX (ft_atoi)
# include <stdio.h>             // printf, readline, perror (must be before readline.h)
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <signal.h>    		// sigaction, sigemptyset, sigaddset
# include <termios.h>			// tcsetattr, tcgetattr, termios
# include <stdbool.h>           // boolean
# include <stdlib.h>            // malloc, free, exit
# include <string.h>			// strerror
# include <sys/stat.h>			// stat
# include <sys/wait.h>			// waitpid
# include <unistd.h>            // read, write, pipe, fork, execve, access, chdir, dup, dup2

# define PROMPT "$> "
# define MINI "minishell"
# define ERR_PTR ((char *)-1)

# define WORD_DELIMITERS " \f\n\r\t\v<>|"
# define OPERATORS "<>|"
# define EXPANDABLE "?_{"
# define QUOTES "\'\""

# define EXIT_CANNOT_EXEC 126
# define EXIT_CMD_NOT_FOUND 127
# define SIG_EXIT_BASE 128
# define EXIT_STATUS_MOD 256
// EXIT_STATUS_MAX = 255
# define EXIT_NUMERIC_ERROR 255

# define E_PARSE "syntax error near unexpected token "
# define E_PATH "command not found"
# define E_ENV "bad substitution"	// ${{name}}　${,name}
# define E_EXIT_CODE "numeric argument required"
# define E_EXPORT "not a valid identifier"
# define E_MANY_ARGS "too many arguments"

extern volatile sig_atomic_t g_signal;

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
	TYPE_REDIR,
	TYPE_ENV
}	e_node_type;

typedef enum
{
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_ENV,
	BUILTIN_UNSET,
	BUILTIN_EXIT
}	e_builtin;

typedef struct s_node
{
	struct s_node *next;
}			t_node;

typedef struct s_sort
{
	void	*base;
	size_t	width;
	int		(*comp)(const void *, const void *);
}			t_sort;

typedef struct s_env
{
    char            *key;
    char            *value;
    bool            exported;
    struct s_env    *next;
}               t_env;

typedef struct s_shell
{
	t_env			*env;
	t_env			*env_tail;
    int				status;
	struct termios	original_term;
}					t_shell;

typedef struct s_path_vars
{
	const char	*path;
    char		*full_path;
    char		*p;
    char		*new_path;
}			t_path_vars;

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

typedef struct s_exec
{
	int				builtin;
	pid_t			pid;	
}					t_exec;

typedef struct s_redir
{
	char				*file;
	bool				quoted;
	e_token_type		flag;
	int					fd;
	struct s_redir		*next;
}	t_redir;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	t_exec			exec;
	struct s_cmd	*next;
}			t_cmd;

typedef struct s_list
{
	void *head;
	void *tail;
	void *new;
}		t_list;

// lexer.c
t_token					*tokenise_input(char *s);
void					lstadd_back(void **tokens, void *new, void *last, e_node_type type);

// parser.c
t_token					*parse_tokens(t_token *tokens, t_shell *shell);

// dollar.c
int						handle_dollar(char **token, char *end, t_string *str, t_shell *shell);

//ast.c
t_cmd					*build_ast(t_token *tokens);

// free.c
void					free_tokens(t_token *tokens, bool free_content, t_token *end);
void					free_cmds(t_cmd *cmds);
t_cmd					*error_free(t_cmd *cmds, t_token *tokens);
void					free_env(t_env *env);
void					free_envp(char **envp);

// heredoc.c
int						process_heredoc(t_cmd *cmds, t_shell *shell);
void					exit_shell(int exit_code, t_cmd *cmds, t_shell *shell);

// execute.c
int						exec_cmds(t_cmd *cmds, t_shell *shell);

// exec_utils.c
int						setup_redirs(t_redir *redirs);
void    				close_pipe_fds(int *fd, int prev_fd);

// exec_child.c
void    				exec_child(t_cmd *cmd, int *fd, int prev_fd, t_shell *shell);

// signal.h
int						setup_handler(int sig, void (*handler)(int));
void					shell_handler(int sig);
void					handle_shell_signal(int *status);
void    				init_signals(struct termios *original_term);

// void    setup_child_handler(int sig);

// builtin.c
int						exec_builtin(t_cmd *cmd, t_shell *shell);
int						exec_echo(char **args);
int						exec_cd(const char *path, t_shell *shell);
int						exec_pwd(void);
int						exec_exit(char **s);

// export.c
int						exec_export(t_shell *shell, char **args);
int						update_env(t_shell *shell, char *arg, t_env *env);
t_env					*env_lookup(t_env *env, const char *arg);
int						is_valid_identifier(char *s);

//export_no_args.c
int						export_no_args(t_env *env);

// env.c
int						exec_env(t_env *env);
const char				*ft_getenv(t_env *env, const char *key);
char    				**env_to_envp(t_env *env);

// unset.c
int						exec_unset(t_shell *shell, char **args);
t_env					*env_lookup_prev(t_env *env, t_env **prev, const char *arg);

// qsort.c
void    				ft_qsort(void *base, size_t nel, size_t width, int (*f)(const void *, const void *));

// utils.c
// remove strrchr?

#endif
