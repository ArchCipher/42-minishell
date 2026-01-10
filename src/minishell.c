/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:07 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 15:55:21 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_list	*parse_input(char *input, t_shell *shell);
static void		init_shell(char **envp, t_shell *shell);
static void		init_shell_terminal(struct termios *original_term);
static void		init_env(t_shell *shell);

/*
	NAME
		minishell

	DESCRIPTION:
		MINISHELL is a minimalistic shell that can be used to execute commands.

		FEATURES:
		• Displays a prompt when waiting for a new command.
		• Has a working history.
		• Searches and launches executables based on PATH variable or using
			relative/absolute paths.
		• Handles single quotes (') which prevent interpretation of meta-chars.
		• Handles double quotes (") which prevent interpretation of meta-chars
			except for $ (dollar sign).
		• Handles environment variables ($VAR) which expand to their values.
		• Handles $? which expands to the exit status of the most recently
			executed foreground pipeline.

		REDIRECTIONS:
		• < redirects input.
		• > redirects output.
		• << (heredoc) reads input until a line containing the delimiter is seen.
		• >> redirects output in append mode.

		PIPES:
		• | connects the output of each command to the input of the next command.

		Signal handling (interactive mode):
		• ctrl-C displays a new prompt on a new line.
		• ctrl-D exits the shell.
		• ctrl-\ does nothing.

		BUILT-IN COMMANDS:
		• echo with option -n
		• cd with only a relative or absolute path
		• pwd with no options
		• export with no options
		• unset with no options
		• env with no options or arguments
		• exit with no options

	EXTERNAL FUNC(S)
		readline, rl_clear_history, rl_on_new_line, rl_replace_line,
			rl_redisplay
		add_history, malloc, free, write, access, open, close, fork, waitpid,
		sigaction, sigemptyset, sigaddset, exit, getcwd, chdir, stat, execve,
		dup, dup2, pipe, strerror, perror, isatty, tcsetattr, tcgetattr

	EXIT STATUS:
		0: success
		1: general error
		126: not executable (insufficient permissions or is a directory)
		127: command not found
		128 + N: fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)
		2 or 255: non numeric argument or numeric > LONG_MAX for exit
			[Linux (2), MacOS (255)]
		258: syntax error near unexpected token
*/

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;
	char	*input;
	t_list	*cmds;

	(void)av;
	if (ac != 1)
		return (perr_msg(E_MANY_ARGS, NULL, NULL, false), 1);
	init_shell(envp, &shell);
	while (1)
	{
		input = readline(PROMPT);
		if (g_signal == SIGINT)
			handle_shell_signal(&shell.status);
		if (!input)
			break ;
		shell.line_num++;
		cmds = parse_input(input, &shell);
		free(input);
		if (cmds)
			shell.status = exec_cmds(cmds, &shell);
		ft_lstclear(&cmds, free_cmd);
	}
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "exit\n", 5);
	exit_shell(NULL, &shell);
}

/*
DESCRIPTION:
	Parses the input string and returns a command tree.
	Returns the command tree on success, NULL on failure.
*/

static t_list	*parse_input(char *input, t_shell *shell)
{
	t_list	*tokens;
	t_list	*tmp;
	t_list	*cmds;

	errno = 0;
	add_history(input);
	tokens = tokenise_input(input);
	tokens = parse_tokens(tokens);
	if (!tokens && errno == EINVAL)
		return (shell->status = EXIT_INVAL_OPTION, NULL);
	tmp = tokens;
	cmds = parse_cmd_list(&tmp);
	if ((!tokens || !cmds) && errno)
		shell->status = 1;
	ft_lstclear(&tokens, free_token);
	if (cmds && process_heredoc(cmds, shell))
		return (ft_lstclear(&cmds, free_cmd), NULL);
	return (cmds);
}

/*
DESCRIPTION:
	Initializes the shell and sets the signal handlers and updates PWD and
	OLDPWD environment variables. In interactive mode, it also sets the
	terminal to not echo control characters.
*/

static void	init_shell(char **envp, t_shell *shell)
{
	if (set_signal_handlers(shell_handler, SIG_IGN))
	{
		perror(MINI);
		exit(1);
	}
	if (isatty(STDIN_FILENO))
		init_shell_terminal(&shell->original_term);
	shell->line_num = 0;
	shell->status = 0;
	shell->env = NULL;
	shell->env_last = NULL;
	while (*envp)
	{
		if (update_env(shell, *envp, NULL))
		{
			perror(MINI);
			shell->status = 1;
			exit_shell(NULL, shell);
		}
		envp++;
	}
	init_env(shell);
}

/*
DESCRIPTION:
	Initializes the signals and sets the terminal to not echo control characters.
	When ECHOCTL flag is set, it prevents tty from printing control characters
	like ^C before handler runs.
*/

static void	init_shell_terminal(struct termios *original_term)
{
	struct termios	term;

	if (tcgetattr(STDIN_FILENO, original_term) == -1)
	{
		perror(MINI);
		exit(1);
	}
	term = *original_term;
	term.c_lflag &= ~ECHOCTL;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) != -1)
		return ;
	perror(MINI);
	tcsetattr(STDIN_FILENO, TCSANOW, original_term);
	exit(1);
}

/*
DESCRIPTION:
	Updates the PWD and OLDPWD environment variables at startup.
*/

static void	init_env(t_shell *shell)
{
	t_list	*last_cmd;

	shell->home = env_lookup(shell->env, "HOME");
	shell->oldpwd = env_lookup(shell->env, "OLDPWD");
	shell->pwd = env_lookup(shell->env, "PWD");
	last_cmd = env_lookup(shell->env, "_");
	if (last_cmd && last_cmd->content)
		get_env(last_cmd)->exported = false;
	if (!shell->oldpwd || !shell->pwd)
		update_pwds(shell);
}
