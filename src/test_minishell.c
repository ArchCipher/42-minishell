/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_minishell.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/11 18:30:00 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/11 18:30:00 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 * TEST VERSION - DO NOT SUBMIT
 * This file is for testing with 42_minishell_tester only.
 * It uses get_next_line when stdin is not a TTY (for automated testing).
 */

#include "minishell.h"

static t_list	*parse_input(char *input, t_shell *shell);
static void		init_shell(char **envp, t_shell *shell);
static void		init_shell_terminal(struct termios *original_term);
static void		init_env(t_shell *shell);

/*
 * Simple get_next_line implementation for testing.
 * Reads a line from fd (stdin) and returns it (without the newline).
 * Returns NULL on EOF or error.
 * 
 * NOTE: If you have your own get_next_line from another project,
 * you can replace this function or link it separately.
 */
static char	*get_next_line(int fd)
{
	char	*line;
	char	buffer[2];
	ssize_t	bytes_read;
	char	*tmp;

	line = ft_strdup("");
	if (!line)
		return (NULL);
	buffer[1] = '\0';
	while (1)
	{
		bytes_read = read(fd, buffer, 1);
		if (bytes_read <= 0)
		{
			if (bytes_read == 0 && ft_strlen(line) > 0)
				return (line);
			free(line);
			return (NULL);
		}
		if (buffer[0] == '\n')
			return (line);
		tmp = ft_strjoin(line, buffer);
		free(line);
		if (!tmp)
			return (NULL);
		line = tmp;
	}
}

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
		if (isatty(STDIN_FILENO))
		{
			input = readline(PROMPT);
			if (g_signal == SIGINT)
				handle_shell_signal(&shell.status);
		}
		else
		{
			input = get_next_line(STDIN_FILENO);
			if (!input)
				break ;
		}
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
	if (isatty(STDIN_FILENO))
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

