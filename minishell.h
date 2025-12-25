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
# include <stdio.h>             // printf, readline, perror (must be before readline.h)
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <signal.h>    		// sigaction, sigemptyset, sigaddset
# include <termios.h>			// tcsetattr, tcgetattr, termios
# include <curses.h>
# include <term.h>
# include <stdbool.h>           // boolean
# include <stdlib.h>            // malloc, free, exit, getenv
# include <string.h>			// strerror
# include <sys/stat.h>			// stat
# include <sys/wait.h>			// waitpid
# include <unistd.h>            // read, write, pipe, fork, execve, access, chdir, dup, dup2

# define PROMPT "$> "
# define MINI "minishell"

# define WORD_DELIMITERS " \f\n\r\t\v<>|"
# define OPERATORS "<>|"
# define EXPANDABLE "?_{"

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
	TYPE_REDIR
}	e_node_type;

typedef enum
{
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_UNSET,
	BUILTIN_ENV,
	BUILTIN_EXIT
}	e_builtin;

typedef struct s_shell
{
    struct termios	original_term;
    int				status;
    char			**envp;
}					t_shell;

typedef struct s_path_vars
{
	char    *path;
    char    *full_path;
    char    *p;
    char    *new_path;
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
t_token					*parse_tokens(t_token *tokens, int status);

//ast.c
t_cmd					*build_ast(t_token *tokens);

// error.c
void					free_tokens(t_token *tokens, bool free_content, t_token *end);
void					free_cmds(t_cmd *cmds);
t_cmd					*error_free(t_cmd *cmds, t_token *tokens);

// heredoc.c
int						process_heredoc(t_cmd *cmds, t_shell *shell);

// execute.c
int						exec_cmds(t_cmd *cmds, char **envp);

// exec_utils.c
int						setup_redirs(t_redir *redirs);
void    				close_pipe_fds(int *fd, int prev_fd);

// exec_child.c
void    				exec_child(t_cmd *cmd, int *fd, int prev_fd, char **envp);

// signal.h
int						setup_handler(int sig, void (*handler)(int));
void					shell_handler(int sig);
void					handle_shell_signal(int *status);
void    				init_signals(struct termios *original_term);

// void    setup_child_handler(int sig);

// builtin.c
int		exec_builtin(t_cmd *cmd);
int		exec_echo(char **args);
int		exec_cd(const char *path);
int		exec_pwd(void);
void	exec_exit(char *s);

// utils.c
int						ft_isspace(int c);
int						ft_isalnum(int c);
void					*ft_memchr(const void *s, int c, size_t n);
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
char					*ft_strjoin(const char *s1, const char *s2);
char					*ft_strjoin3(const char *s1, const char *s2, const char *s3);

#endif