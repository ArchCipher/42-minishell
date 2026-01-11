/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   types.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/01 16:43:39 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 19:18:31 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TYPES_H
# define TYPES_H

# include <stdbool.h> // boolean
# include <termios.h> // tcsetattr, tcgetattr, termios, ECOCTL

typedef enum t_token_type
{
	NONE,
	WORD,
	SQUOTE,
	DQUOTE,
	TARGET_FD,
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
	EMPTY_STRING,
	BUILTIN_ECHO,
	BUILTIN_CD,
	BUILTIN_PWD,
	BUILTIN_EXPORT,
	BUILTIN_ENV,
	BUILTIN_UNSET,
	BUILTIN_EXIT
}					t_builtin;

typedef struct s_shell
{
	t_list			*env;
	t_list			*env_last;
	t_list			*home;
	t_list			*oldpwd;
	t_list			*pwd;
	int				status;
	char			exit_code[4];
	int				line_num;
	struct termios	original_term;
}					t_shell;

typedef struct s_env
{
	char			*key;
	char			*value;
	bool			exported;
}					t_env;

typedef struct s_list_info
{
	t_list			*head;
	t_list			*last;
	t_list			*new;
}					t_list_info;

typedef struct s_token
{
	char			*raw;
	char			*word;
	t_token_type	type;
	size_t			len;
	bool			quoted;
}					t_token;

typedef struct s_redir
{
	char			*file;
	bool			quoted;
	t_token_type	flag;
	int				fd;
	int				target_fd;
}					t_redir;

typedef struct s_exec
{
	int				builtin;
	pid_t			pid;
	int				prev_fd;	
}					t_exec;

typedef struct s_cmd
{
	char			**args;
	size_t			arglen;
	size_t			argcap;
	t_list			*redirs;
	t_exec			exec;
	t_token_type	con;
	t_list			*subshell;
}					t_cmd;

typedef struct s_strtok
{
	const char		*token;
	char			*p;
	char			*str;
}					t_strtok;

typedef struct s_expand
{
	char			*dst;
	size_t			len;
	size_t			cap;
	char			*src;
	char			*src_end;
}					t_expand;

#endif

