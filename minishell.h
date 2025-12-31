/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:44:56 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "types.h"
# include "ft_dprintf.h"
# include "libft.h"
# include <errno.h>		// errno
# include <fcntl.h>		// open
# include <limits.h>	// LONG_MAX (ft_atoi)
// stdio.h must be included before readline.h
# include <stdio.h>    // readline, perror
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline
# include <stdlib.h>   // malloc, free, exit
# include <string.h>   // strerror
# include <sys/stat.h> // stat
# include <sys/wait.h> // waitpid
# include <unistd.h>
// write, access, close, fork, getcwd, chdir, execve, dup, dup2, pipe, isatty

# define PROMPT "$> "
# define MINI "minishell"
# define ERR_PTR ((char *)-1) // used as a sentinal value for invalid substitution.

# define WORD_DELIMITERS " \f\n\r\t\v<>|"
# define OPERATORS "<>|"
# define EXPANDABLE "?_{"
# define QUOTES "\'\""

# define EXIT_CANNOT_EXEC 126
# define EXIT_CMD_NOT_FOUND 127
# define SIG_EXIT_BASE 128
# define EXIT_STATUS_MOD 256
# define EXIT_NUMERIC_ERROR 255

# define E_PARSE "syntax error near unexpected token "
# define E_PATH "command not found"
# define E_DIR "is a directory"
# define E_ENV "bad substitution"	// invalid substitution
# define E_EXIT_CODE "numeric argument required"
# define E_EXPORT "not a valid identifier"
# define E_MANY_ARGS "too many arguments"

extern volatile sig_atomic_t	g_signal;

// typedef struct s_node
// {
// 	struct s_node				*next;
// }								t_node;

// Parsing (lexer, list, parser, var, dollar, ast)
t_token							*tokenise_input(char *s);

void							lstadd_back(void **tokens, void *new,
									void *last, e_node_type type);

t_token							*parse_tokens(t_token *tokens, t_shell *shell);

const char						*get_var(char **token, char *end, t_shell *shell);

int								append_var(t_string *str, const char *var, size_t last);
bool							dollar_expandable(char *s, char *end);

t_cmd							*build_ast(t_token *tokens);

// Execution (exec, exec_child, path, redirs, heredoc)
int								exec_cmds(t_cmd *cmds, t_shell *shell);

int								fork_with_pipe(t_cmd *cmd, int *prev_fd, t_shell *shell);

char							*get_valid_path(const char *filename, t_env *env);

int								setup_redirs(t_redir *redirs);
void							close_pipe_fds(int *fd, int prev_fd);
int								close_fds_error(int *fd, int prev_fd);

int								process_heredoc(t_cmd *cmds, t_shell *shell);

// Builtin (builtin, exit, export, unset, env_utils,qsort)
int								exec_builtin(t_cmd *cmd, t_shell *shell);
int								exec_echo(char **args);
int								exec_cd(const char *path, t_shell *shell);
int								exec_pwd(void);
int								exec_env(t_env *env);

void							exit_shell(int exit_code, t_cmd *cmds,
									t_shell *shell);
int								exec_exit(char **s);

int								exec_export(t_shell *shell, char **args);
int								is_valid_identifier(char *s);

int								exec_unset(t_shell *shell, char **args);

const char						*ft_getenv(t_env *env, const char *key);
t_env							*env_lookup(t_env *env, const char *arg);
t_env							*env_lookup_prev(t_env *env, t_env **prev,
									const char *arg);
int								update_env(t_shell *shell, char *arg,
									t_env *env);

void							ft_qsort_env(t_env **arr, size_t low, size_t high);

// Utils (signal, free)
int								set_signal_handlers(void (*sigint)(int), void (*sigquit)(int));
int								set_signal_handler(int sig, void (*handler)(int));
void							shell_handler(int sig);
void							handle_shell_signal(int *status);

void							free_tokens(t_token *tokens, bool free_content,
									t_token *end);
void							free_cmds(t_cmd *cmds);
t_cmd							*error_free(t_cmd *cmds, t_token *tokens);
void							free_env(t_env *env);
void							free_envp(char **envp);

#endif
