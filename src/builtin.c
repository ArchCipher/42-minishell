/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:01:35 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:56:00 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
DESCRIPTION:
	Executes the builtin command and returns the exit status of the builtin.
*/

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (cmd->exec.builtin == BUILTIN_ECHO)
		return (exec_echo(cmd->args + 1));
	else if (cmd->exec.builtin == BUILTIN_CD)
		return (exec_cd(cmd->args, shell, env_lookup(shell->env, "PWD"),
				env_lookup(shell->env, "OLDPWD")));
	else if (cmd->exec.builtin == BUILTIN_PWD)
		return (exec_pwd());
	else if (cmd->exec.builtin == BUILTIN_EXPORT)
		return (exec_export(*cmd->args, cmd->args + 1, shell));
	else if (cmd->exec.builtin == BUILTIN_ENV)
		return (exec_env(shell->env));
	else if (cmd->exec.builtin == BUILTIN_UNSET)
		return (exec_unset(*cmd->args, cmd->args + 1, shell));
	else if (cmd->exec.builtin == BUILTIN_EXIT)
		return (exec_exit(cmd->args));
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

int	exec_echo(char **args)
{
	bool	nl;

	nl = true;
	if (!args || !*args)
		return (write(1, "\n", 1), 0);
	if (ft_strcmp(*args, "-n") == 0)
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
	Executes the cd builtin command using chdir().
	Updates the PWD and OLDPWD environment variables if PWD and OLDPWD exist.
	Returns 0 on success, 1 if chdir() or getcwd() fails.
*/

int	exec_cd(char **args, t_shell *shell, t_env *pwd, t_env *old_pwd)
{
	const char	*dir;

	if (args[1] && args[2])
		return (perr_msg(*args, E_MANY_ARGS, NULL), 1);
	dir = args[1];
	if (!dir || (*dir == '~' && !dir[1]))
	{
		dir = ft_getenv(shell->env, "HOME");
		if (!dir)
			return (perr_msg(*args, E_HOME, NULL), 1);
	}
	if (chdir(dir) == -1)
		return (perr_msg(*args, args[1], strerror(errno)), 1);
	if (!pwd || !pwd->value)
		return (0);
	if (old_pwd)
	{
		free(old_pwd->value);
		old_pwd->value = pwd->value;
		old_pwd->exported = true;
	}
	else if (pwd->value)
		free(pwd->value);
	pwd->value = getcwd(NULL, 0);
	return (0);
}

/*
DESCRIPTION:
	Retrieves pwd using getcwd() and writes it to stdout.
	Returns 0 on success, 1 if getcwd() or write() fails.
*/

int	exec_pwd(void)
{
	char	*buf;

	buf = NULL;
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

int	exec_env(t_env *env)
{
	int	ret;

	ret = 0;
	while (env)
	{
		if (env->exported && env->value && ft_dprintf(STDOUT_FILENO, "%s=%s\n",
				env->key, env->value) < 0)
			ret = 1;
		env = env->next;
	}
	return (ret);
}
