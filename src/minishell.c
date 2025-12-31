/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/04 11:27:07 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:52:17 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_cmd *parse_input(char *input, t_shell *shell);
static void	init_shell(char **envp, t_shell *shell);
static void	init_shell_terminal(struct termios *original_term);
static void	update_pwd(t_shell *shell);

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
		• Handles single quotes (') which prevent interpretation of meta-characters.
		• Handles double quotes (") which prevent interpretation of meta-characters
		  except for $ (dollar sign).
		• Handles environment variables ($VAR) which expand to their values.
		• Handles $? which expands to the exit status of the most recently executed
		  foreground pipeline.
		
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
		readline, rl_clear_history, rl_on_new_line, rl_replace_line, rl_redisplay
		add_history, malloc, free, write, access, open, close, fork, waitpid,
		sigaction, sigemptyset, sigaddset, exit, getcwd, chdir, stat, execve,
		dup, dup2, pipe, strerror, perror, isatty, tcsetattr, tcgetattr

	EXIT STATUS:
		0: success
		1: general error
		126: not executable (insufficient permissions or is a directory)
		127: command not found
		128 + N: fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)
		255: non numeric argument for exit
*/

int	main(int ac, char **av, char **envp)
{
	t_shell	shell;
	char	*input;
	t_cmd	*cmds;

	(void)av;
	if (ac != 1)
		return (ft_dprintf(STDERR_FILENO, "%s: %s\n", MINI, E_MANY_ARGS), 1);
	init_shell(envp, &shell);
	input = readline(PROMPT);
	while (input)
	{
		if (g_signal == SIGINT)
			handle_shell_signal(&shell.status);
		add_history(input);
		cmds = parse_input(input, &shell);
		free(input);
		if (cmds && !process_heredoc(cmds, &shell))
			shell.status = exec_cmds(cmds, &shell);
		free_cmds(cmds);
		input = readline(PROMPT);	
	}
	if (isatty(STDIN_FILENO))
		write(STDOUT_FILENO, "exit\n", 5);
	exit_shell(shell.status, NULL, &shell);
}

/*
DESCRIPTION:
	Parses the input string and returns a command tree.
	Returns the command tree on success, NULL on failure.
*/

static t_cmd *parse_input(char *input, t_shell *shell)
{
    t_token *tokens;
    t_cmd *cmds;

    tokens = tokenise_input(input);
    tokens = parse_tokens(tokens, shell);
    cmds = build_ast(tokens);
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
	update_pwd(shell);
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

static void	update_pwd(t_shell *shell)
{
	t_env	*old_pwd;
	t_env	*pwd;

	old_pwd = env_lookup(shell->env, "OLDPWD");
	if (!old_pwd)
	{
		if (update_env(shell, "OLDPWD", NULL))
			return ;
		old_pwd = env_lookup(shell->env, "OLDPWD");
	}
	old_pwd->exported = false;
	if (old_pwd->value)
		free(old_pwd->value);
	old_pwd->value = NULL;
	pwd = env_lookup(shell->env, "PWD");
	if (!pwd)
	{
		if (update_env(shell, "PWD", NULL))
			return ;
		pwd = env_lookup(shell->env, "PWD");
	}
	if (pwd->value)
		free(pwd->value);
	pwd->value = getcwd(NULL, 0);
}
