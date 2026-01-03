/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:39:50 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "ft_dprintf.h"
# include "libft.h"
# include "types.h"
# include <errno.h>  // errno
# include <fcntl.h>  // open
# include <limits.h> // LONG_MAX (ft_atoi)
// stdio.h must be included before readline.h
# include <stdio.h>             // readline, perror
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <stdlib.h>            // malloc, free, exit
# include <string.h>            // strerror
# include <sys/stat.h>          // stat
# include <sys/wait.h>          // waitpid
# include <unistd.h>
// write, access, close, fork, getcwd, chdir, execve, dup, dup2, pipe, isatty

# define FD_IN STDIN_FILENO
# define FD_OUT STDOUT_FILENO
# define FD_ERR STDERR_FILENO

# define PROMPT "$> "
# define MINI "minishell"
// used as a sentinal value for invalid substitution.

// # define WORD_DELIMITERS " \f\n\r\t\v<>|"
// # define OPERATORS "<>|&"	// ()"
# define EXPANDABLE "?_{"
// # define QUOTES "\'\""

# define EXIT_CANNOT_EXEC 126
# define EXIT_CMD_NOT_FOUND 127
# define SIG_EXIT_BASE 128
# define EXIT_STATUS_MOD 256
# ifdef __linux__
#  define EXIT_NUMERIC_ERROR 2
# else
#  define EXIT_NUMERIC_ERROR 255
# endif
# define E_PARSE "syntax error near unexpected token"
# define E_PATH "command not found"
# define E_DIR "is a directory"
# define E_ENV "bad substitution" // invalid substitution
# define E_EXIT_CODE "numeric argument required"
# define E_EXPORT "not a valid identifier"
# define E_MANY_ARGS "too many arguments"
# define E_HOME "HOME not set"

extern volatile sig_atomic_t	g_signal;

// typedef struct s_node
// {
// 	struct s_node				*next;
// }								t_node;

// Parsing (lexer, list, parser, var, dollar, ast)
t_token							*tokenise_input(char *s);

void							lstadd_back(void **tokens, void *new,
									void *last, t_node_type type);

t_token							*parse_tokens(t_token *tokens, t_shell *shell);

const char						*get_var(char **token, char *end,
									t_shell *shell);

int								append_var(t_string *str, const char *var,
									size_t last);
bool							dollar_expandable(char *s, char *end);

// t_cmd							*build_ast(t_token *tokens);
t_cmd							*parse_cmd_list(t_token **tok);

// Execution (exec, exec_child, path, redirs, heredoc)
int								exec_cmds(t_cmd *cmds, t_shell *shell);

int								fork_with_pipe(t_cmd *cmd, t_shell *shell);

char							*get_valid_path(const char *filename,
									t_env *env);

int								setup_redirs(t_redir *redirs);
void							close_pipe_fds(int *fd, int prev_fd);
int								close_fds_error(int *fd, int prev_fd);

int								process_heredoc(t_cmd *cmds, t_shell *shell);

// Builtin (builtin, exit, export, unset, env_utils,qsort)
int								exec_builtin(t_cmd *cmd, t_shell *shell);
int								exec_echo(char **args);
int								exec_cd(char **args, t_shell *shell);
void							update_pwds(t_shell *shell);
int								exec_pwd(void);
int								exec_env(t_env *env);

void							exit_shell(int exit_code, t_cmd *cmds,
									t_shell *shell);
int								exec_exit(char **s);

int								exec_export(char **args, t_shell *shell);
int								is_valid_identifier(const char *s);

int								exec_unset(char **args, t_shell *shell);

const char						*ft_getenv(t_env *env, const char *key);
t_env							*env_lookup(t_env *env, const char *arg);
t_env							*env_lookup_prev(t_env *env, t_env **prev,
									const char *arg);
int								update_env(t_shell *shell, const char *arg,
									t_env *env);

void							ft_qsort_env(t_env **arr, size_t low,
									size_t high);

// Utils (signal, free, error)
int								set_signal_handlers(void (*sigint)(int),
									void (*sigquit)(int));
int								set_signal_handler(int sig,
									void (*handler)(int));
void							shell_handler(int sig);
void							handle_shell_signal(int *status);

void							free_tokens(t_token *tokens, bool free_content,
									t_token *end);
void							free_args(char **args);
void							free_cmds(t_cmd *cmds);
void							free_env(t_env *env);
void							free_envp(char **envp);

void							perr_msg(const char *s1, const char *s2,
									const char *s3, bool backtick);
void							perr_token(char *s, size_t	len);
t_cmd							*free_cmds_err(t_cmd *cmds, t_token *tokens);

int								is_redir(char c);
int								is_connector(char *s);
int								is_parenthesis(char c);
int								is_word_delimiter(char *s);
t_token_type					get_flag(char *s, t_token_type flag);

int type_redir(t_token_type t);
int type_con(t_token_type t);

#endif
