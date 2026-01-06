/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:33:15 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 21:50:53 by kmurugan         ###   ########.fr       */
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
# include <signal.h>            // sigaction, sigemptyset, sigaddset
# include <stdlib.h>            // malloc, free, exit
# include <string.h>            // strerror
# include <sys/stat.h>          // stat
# include <sys/wait.h>          // waitpid
# include <unistd.h>
// write, access, close, fork, getcwd, chdir, execve, dup, dup2, pipe, isatty

# define PROMPT "$> "
# define MINI "minishell"
# define EXPANDABLE "?_{"
# define IS_SPACE " \t\n\v\f\r"

# define E_REDIR "ambiguous redirect"
# define E_ENV "bad substitution"
# define E_EXPORT "not a valid identifier"
# define E_MANY_ARGS "too many arguments"
# define E_HOME "HOME not set"
# define E_OLDPWD "OLDPWD not set"

// heredoc EOF warning msg on linux
# define W_EOF1 "warning: here-document at line"
# define W_EOF2 "delimited by end-of-file"

# define EXIT_INVAL_OPTION 258
// invalid option exits with 2, minishell doesn't support options
# define E_OPTION "does not support options"
# define E_PARSE "syntax error near unexpected token"
# define E_ARG "filename argument required"

# define EXIT_CANNOT_EXEC 126
# define E_DIR "is a directory"

# define EXIT_CMD_NOT_FOUND 127
# define E_CMD "command not found"

# define SIG_EXIT_BASE 128
# define EXIT_STATUS_MASK 0xFF

# ifdef __linux__
#  define EXIT_NUMERIC_ERROR 2
# else
#  define EXIT_NUMERIC_ERROR 255
# endif
# define E_EXIT_CODE "numeric argument required"

extern volatile sig_atomic_t	g_signal;

// Parsing
t_token							*tokenise_input(char *s);
t_token							*parse_tokens(t_token *tokens, t_shell *shell);
t_cmd							*parse_cmd_list(t_token **tok);
int								build_cmd(t_token **tok, t_cmd *cmd,
									void **last_redir);
int								expand_word_token(t_token *tok, t_token *prev,
									t_shell *shell);
int								split_word_token(t_token **head, t_token **cur,
									t_token *prev);
void							del_one_token(t_token **head, t_token *prev,
									t_token **cur);
t_token							*create_token(void *token, t_token_type type,
									size_t len);
void							free_tokens(t_token *tokens);
int								is_redir(char c);
int								is_connector(char *s);
int								is_parenthesis(char c);
int								is_word_delimiter(char *s);
int								is_type_redir(t_token_type t);
int								is_type_con(t_token_type t);
t_token_type					update_quote_flag(char c, t_token_type flag);
void							perr_token(char *s, size_t len);
void							perr_tok_msg(char *cmd, char *s, size_t len,
									const char *msg);

// Execution
int								process_heredoc(t_cmd *cmds, t_shell *shell);
int								exec_cmds(t_cmd *cmds, t_shell *shell);
int								fork_with_pipe(t_cmd *cmd, t_shell *shell);
char							*get_valid_path(const char *filename,
									t_env *env);
int								setup_redirs(t_redir *redirs);
void							close_pipe_fds(int *fd, int prev_fd);
int								close_fds_error(int *fd, int prev_fd);
int								exec_builtin(t_cmd *cmd, t_shell *shell);
void							exit_shell(int exit_code, t_cmd *cmds,
									t_shell *shell);
void							free_cmds(t_cmd *cmds);
void							free_arr(char **envp);

// Builtin
int								exec_pwd(void);
int								exec_cd(char **args, t_shell *shell);
int								exec_exit(char **s);
int								exec_export(char **args, t_shell *shell);
int								exec_unset(char **args, t_shell *shell);
void							ft_qsort_env(t_env **arr, size_t low,
									size_t high);

// Environment
const char						*ft_getenv(t_env *env, const char *key);
t_env							*env_lookup(t_env *env, const char *arg);
t_env							*env_lookup_prev(t_env *env, t_env **prev,
									const char *arg);
int								update_env(t_shell *shell, const char *arg,
									t_env *env);
void							update_pwds(t_shell *shell);
int								is_valid_identifier(const char *s);

// Expansion
t_string						alloc_tstring(size_t size);
const char						*get_var(char **token, char *end,
									t_shell *shell);
int								append_var(t_string *str, char **token,
									char *end, t_shell *shell);
int								dollar_expandable(char *s, char *end);

// Signal
int								set_signal_handlers(void (*sigint)(int),
									void (*sigquit)(int));
int								set_signal_handler(int sig,
									void (*handler)(int));
void							shell_handler(int sig);
void							handle_shell_signal(int *status);

// Utils
void							lstadd_back(void **tokens, void *new,
									void *last, t_node_type type);
void							perr_msg(const char *s1, const char *s2,
									const char *s3, bool backtick);

#endif
