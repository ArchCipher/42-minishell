/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:07 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:06:20 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_cmd	*parse_input(char *input, t_shell *shell);
static void		init_shell(char **envp, t_shell *shell);
static void		init_shell_terminal(struct termios *original_term);
static void		init_env(t_shell *shell);
static char		*get_next_line(int fd);

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
		2: non numeric argument for exit on Linux
		126: not executable (insufficient permissions or is a directory)
		127: command not found
		128 + N: fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)
		255: non numeric argument for exit on MacOS
*/

int	main(int ac, char **av, char **envp)
{
	t_shell		shell;
	char		*input;
	t_cmd		*cmds;

	(void)av;
	if (ac != 1)
		return (perr_msg(E_MANY_ARGS, NULL, NULL, false), 1);
	init_shell(envp, &shell);
	if (isatty(STDIN_FILENO))
		input = readline(PROMPT);
	else
		input = get_next_line(STDIN_FILENO);
	while (input)
	{
		if (g_signal == SIGINT)
			handle_shell_signal(&shell.status);
		if (isatty(STDIN_FILENO))
			add_history(input);
		cmds = parse_input(input, &shell);
		free(input);
		if (cmds && !process_heredoc(cmds, &shell))
			shell.status = exec_cmds(cmds, &shell);
		free_cmds(cmds);
		if (isatty(STDIN_FILENO))
			input = readline(PROMPT);
		else
			input = get_next_line(STDIN_FILENO);
	}
	// if (isatty(STDIN_FILENO))
	// 	write(STDOUT_FILENO, "exit\n", 5);
	exit_shell(shell.status, NULL, &shell);
}

/*
DESCRIPTION:
	Parses the input string and returns a command tree.
	Returns the command tree on success, NULL on failure.
*/

static t_cmd	*parse_input(char *input, t_shell *shell)
{
	t_token	*tokens;
	t_token	*tmp;
	t_cmd	*cmds;

	tokens = tokenise_input(input);
	tokens = parse_tokens(tokens, shell);
	tmp = tokens;
	cmds = parse_cmd_list(&tmp);
	free_tokens(tokens, true, NULL);
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
	shell->status = 0;
	shell->env = NULL;
	while (*envp)
	{
		if (update_env(shell, *envp, NULL))
		{
			perror(MINI);
			exit_shell(1, NULL, shell);
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
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		perror(MINI);
		tcsetattr(STDIN_FILENO, TCSANOW, original_term);
		exit(1);
	}
}

/*
DESCRIPTION:
	Updates the PWD and OLDPWD environment variables at startup.
*/

/*
DESCRIPTION:
	Reads a line from the file descriptor (non-interactive mode).
	Returns the line on success (without trailing newline), NULL on EOF or error.
	The returned string must be freed.
*/

static char	*get_next_line(int fd)
{
	char	*line;
	char	buf[2];
	ssize_t	bytes_read;
	size_t	len;
	char	*tmp;

	line = NULL;
	len = 0;
	buf[1] = '\0';
	while ((bytes_read = read(fd, buf, 1)) > 0)
	{
		if (buf[0] == '\n')
			break ;
		tmp = line;
		line = malloc(len + 2);
		if (!line)
			return (free(tmp), NULL);
		if (tmp)
		{
			ft_memcpy(line, tmp, len);
			free(tmp);
		}
		line[len] = buf[0];
		len++;
		line[len] = '\0';
	}
	if (bytes_read == -1)
		return (free(line), NULL);
	return (line);
}

static void	init_env(t_shell *shell)
{
	// t_env	*last_cmd;

	shell->home = env_lookup(shell->env, "HOME");
	shell->oldpwd = env_lookup(shell->env, "OLDPWD");
	shell->pwd = env_lookup(shell->env, "PWD");
	// last_cmd = env_lookup(shell->env, "_");
	// if (last_cmd)
	// 	last_cmd->exported = false;
	update_pwds(shell);
	if (shell->oldpwd && shell->oldpwd->value)
	{
		free(shell->oldpwd->value);
		shell->oldpwd->value = NULL;
	}
}
