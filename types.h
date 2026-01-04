/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:43:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 17:32:48 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include <signal.h>  // pid_t, sigaction, sigemptyset, sigaddset
# include <stdbool.h> // boolean
# include <termios.h> // tcsetattr, tcgetattr, termios, ECOCTL

typedef enum t_token_type
{
	NONE,
	WORD,
	SQUOTE,
	DQUOTE,
	REDIR_IN,
	REDIR_OUT,
	APPEND,
	HEREDOC,
	PIPE_CHAR,
	AND,
	OR,
	L_PAREN,
	R_PAREN
}					t_token_type;

// for wildcard (*) check glob()

typedef enum t_builtin
{
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_ENV,
	BUILTIN_UNSET,
	BUILTIN_EXIT
}					t_builtin;

typedef enum t_node_type
{
	TYPE_TOKEN,
	TYPE_CMD,
	TYPE_REDIR,
	TYPE_ENV
}					t_node_type;

typedef struct s_env
{
	char			*key;
	char			*value;
	bool			exported;
	struct s_env	*next;
}					t_env;

typedef struct s_shell
{
	t_env			*env;
	t_env			*env_tail;
	t_env			*home;
	t_env			*oldpwd;
	t_env			*pwd;
	int				status;
	struct termios	original_term;
}					t_shell;

typedef struct s_token
{
	t_token_type	type;
	char			*token;
	bool			quoted;
	size_t			len;
	struct s_token	*next;
}					t_token;

typedef struct s_string
{
	char			*s;
	size_t			len;
	size_t			cap;
}					t_string;

typedef struct s_list
{
	void			*head;
	void			*tail;
	void			*new;
}					t_list;

typedef struct s_redir
{
	char			*file;
	bool			quoted;
	t_token_type	flag;
	int				fd;
	struct s_redir	*next;
}					t_redir;

typedef struct s_exec
{
	int				builtin;
	pid_t			pid;
	int				p_fd;	
}					t_exec;

typedef struct s_cmd
{
	char			**args;
	t_redir			*redirs;
	t_exec			exec;
	t_token_type	con;
	struct s_cmd	*next;
	struct s_cmd	*sub;
}					t_cmd;

typedef struct s_strtok
{
	const char		*token;
	char			*p;
	char			*str;
}					t_strtok;

#endif
