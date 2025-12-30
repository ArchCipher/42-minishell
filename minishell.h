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

// split to 2 files??

#ifndef MINISHELL_H
# define MINISHELL_H

# include "types.h"
# include "ft_dprintf.h"
# include "libft.h"
# include <errno.h>             // errno
# include <fcntl.h>             // open
# include <limits.h>            // LONG_MAX (ft_atoi)
// must be before readline.h
# include <stdio.h>    // printf, readline, perror
# include <readline/history.h>  // readline (linux)
# include <readline/readline.h> // readline

# include <stdlib.h>   // malloc, free, exit
# include <string.h>   // strerror
# include <sys/stat.h> // stat
# include <sys/wait.h> // waitpid

# include <unistd.h>
// read, write, pipe, fork, execve, access, chdir, dup, dup2, isatty

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
# define EXIT_NUMERIC_ERROR 255

# define E_PARSE "syntax error near unexpected token "
# define E_PATH "command not found"
# define E_DIR "is a directory"
# define E_ENV "bad substitution" // ${{name}}　${,name}
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

int								copy_var(t_string *str, const char *var, size_t last);
bool							dollar_expandable(char *s, char *end);

t_cmd							*build_ast(t_token *tokens);

// Execution (heredoc, execute, redirs, exec_child, path)
int								process_heredoc(t_cmd *cmds, t_shell *shell);
void							exit_shell(int exit_code, t_cmd *cmds,
									t_shell *shell);

int								exec_cmds(t_cmd *cmds, t_shell *shell);

int								setup_redirs(t_redir *redirs);
void							close_pipe_fds(int *fd, int prev_fd);

int								fork_with_pipe(t_cmd *cmd, int *prev_fd, t_shell *shell);

char							*get_valid_path(const char *filename, t_env *env);

// Builtin (builtin, export, export_no_args, env, unset, qsort)
int								exec_builtin(t_cmd *cmd, t_shell *shell);
int								exec_echo(char **args);
int								exec_cd(const char *path, t_shell *shell);
int								exec_pwd(void);
int								exec_exit(char **s);

int								exec_export(t_shell *shell, char **args);
int								update_env(t_shell *shell, char *arg,
									t_env *env);
int								is_valid_identifier(char *s);
t_env							*env_lookup(t_env *env, const char *arg);

int								export_no_args(t_env *env);

int								exec_env(t_env *env);
const char						*ft_getenv(t_env *env, const char *key);

int								exec_unset(t_shell *shell, char **args);
t_env							*env_lookup_prev(t_env *env, t_env **prev,
									const char *arg);

void							ft_qsort_env(t_env **arr, size_t low, size_t high);

// Utils (signal, free)
int								setup_handler(int sig, void (*handler)(int));
void							shell_handler(int sig);
void							handle_shell_signal(int *status);
void							init_signals(struct termios *original_term);
void							setup_child_handler();

void							free_tokens(t_token *tokens, bool free_content,
									t_token *end);
void							free_cmds(t_cmd *cmds);
t_cmd							*error_free(t_cmd *cmds, t_token *tokens);
void							free_env(t_env *env);
void							free_envp(char **envp);

#endif
