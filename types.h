#ifndef TYPES_H
# define TYPES_H

# include <stdbool.h>   // boolean
# include <signal.h>    // pid_t, sigaction, sigemptyset, sigaddset
# include <termios.h>   // tcsetattr, tcgetattr, termios, ECOCTL

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
}								e_token_type;

// for wildcard (*) check glob()

typedef enum
{
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_ENV,
	BUILTIN_UNSET,
	BUILTIN_EXIT
}								e_builtin;

typedef enum
{
	TYPE_TOKEN,
	TYPE_CMD,
	TYPE_REDIR,
	TYPE_ENV
}								e_node_type;

typedef struct s_env
{
	char						*key;
	char						*value;
	bool						exported;
	struct s_env				*next;
}								t_env;

typedef struct s_shell
{
	t_env						*env;
	t_env						*env_tail;
	int							status;
	struct termios				original_term;
}								t_shell;

typedef struct s_token
{
	e_token_type				type;
	char						*token;
	bool						quoted;
	size_t						len;
	struct s_token				*next;
}								t_token;

typedef struct s_string
{
	char						*s;
	size_t						len;
	size_t						cap;
}								t_string;

typedef struct s_list
{
	void						*head;
	void						*tail;
	void						*new;
}								t_list;

typedef struct s_redir
{
	char						*file;
	bool						quoted;
	e_token_type				flag;
	int							fd;
	struct s_redir				*next;
}								t_redir;

typedef struct s_exec
{
	int							builtin;
	pid_t						pid;
}								t_exec;

typedef struct s_cmd
{
	char						**args;
	t_redir						*redirs;
	t_exec						exec;
	struct s_cmd				*next;
}								t_cmd;


typedef struct s_strtok
{
	const char					*token;
	char						*p;
	char						*str;
}								t_strtok;

#endif