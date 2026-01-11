/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:01:35 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/11 16:13:47 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	exec_echo(char **args);
static int	exec_env(char **args, t_list *envs);

/*
DESCRIPTION:
	Executes the builtin command and returns the exit status of the builtin.
*/

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (cmd->exec.builtin == NONE)
		return (0);
	if (cmd->exec.builtin == EMPTY_STRING)
		return (perr_msg(*cmd->args, NULL, E_CMD, false), EXIT_CMD_NOT_FOUND);
	if (cmd->exec.builtin == BUILTIN_ECHO)
		return (exec_echo(cmd->args));
	else if (cmd->exec.builtin == BUILTIN_CD)
		return (exec_cd(cmd->args, shell));
	else if (cmd->exec.builtin == BUILTIN_PWD)
		return (exec_pwd(cmd->args));
	else if (cmd->exec.builtin == BUILTIN_EXPORT)
		return (exec_export(cmd->args, shell));
	else if (cmd->exec.builtin == BUILTIN_ENV)
		return (exec_env(cmd->args, shell->env));
	else if (cmd->exec.builtin == BUILTIN_UNSET)
		return (exec_unset(cmd->args, shell));
	else if (cmd->exec.builtin == BUILTIN_EXIT)
		return (exec_exit(cmd->args, shell));
	return (0);
}

/*
DESCRIPTION:
	Writes the arguments to stdout.
	Returns 0 on success, 1 if write() fails.

NOTE:
	For echo -n to infile, write() should be used instead of printf().
	printf() needs to be fflush(force flushed) to make it work.
	printf() is part of C standard I/O (stdio), which uses buffers:
		stdout is line-buffered when connected to a terminal
		stdout is fully-buffered when connected to a file or pipe
	Buffered data is only flushed on newline (\n) if line-buffered or when buffer
	is full or when you explicitly call fflush(stdout)
*/

static int	exec_echo(char **args)
{
	bool	nl;

	args++;
	nl = true;
	if (!*args)
		return (write(STDOUT_FILENO, "\n", 1), 0);
	while (*args && **args == '-' && (*args)[1] == 'n'
		&& (ft_strspn(*args + 2, "n") == ft_strlen(*args + 2)))
	{
		nl = false;
		args++;
	}
	while (*args)
	{
		if (ft_putstr(*args, STDOUT_FILENO))
			return (1);
		if (args[1] && write(STDOUT_FILENO, " ", 1) == -1)
			return (1);
		args++;
	}
	if (nl && write(STDOUT_FILENO, "\n", 1) == -1)
		return (1);
	return (0);
}

/*
DESCRIPTION:
	Retrieves pwd using getcwd() and writes it to stdout.
	Returns 0 on success, 1 if getcwd() or write() fails.
*/

int	exec_pwd(char **args)
{
	char	*buf;

	buf = NULL;
	if (args[1] && args[1][0] == '-' && args[1][1] && (args[1][1] != '-'
		|| (args[1][1] == '-' && args[1][2])))
		return (perr_tok_msg(*args, args[1], 2, E_OPTION), 2);
	buf = getcwd(buf, 0);
	if (!buf)
		return (perror(MINI), 1);
	if (ft_putstr(buf, STDOUT_FILENO) < 0 || write(STDOUT_FILENO, "\n", 1) < 0)
		return (perror(MINI), free(buf), 1);
	free(buf);
	return (0);
}

/*
DESCRIPTION:
	Prints the exported environment variables to stdout.
	Returns 0 on success, 1 if ft_dprintf() fails.

FORMAT:
	KEY=value

NOTE:
	ft_dprintf() is used instead of printf() to avoid buffer flushing issues.
*/

static int	exec_env(char **args, t_list *envs)
{
	t_env	*env;
	int		ret;

	if (args[1] && args[1][0] == '-' && !args[1][1])
		return (0);
	if (args[1] && args[1][0] == '-' && args[1][1] && (args[1][1] != '-'
		|| (args[1][1] == '-' && args[1][2])))
		return (perr_msg(*args, args[1], E_OPTION, false), 2);
	ret = 0;
	while (envs)
	{
		env = get_env(envs);
		if (env->exported && env->value && ft_dprintf(STDOUT_FILENO, "%s=%s\n",
				env->key, env->value) < 0)
			ret = 1;
		envs = envs->next;
	}
	return (ret);
}
