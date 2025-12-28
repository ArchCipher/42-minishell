/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/10 14:01:35 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:51:31 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exec_builtin(t_cmd *cmd, t_shell *shell)
{
	if (cmd->exec.builtin == BUILTIN_ECHO)
		return (exec_echo(cmd->args + 1));
	else if (cmd->exec.builtin == BUILTIN_CD)
		return (exec_cd(cmd->args[1], shell));
	else if (cmd->exec.builtin == BUILTIN_PWD)
		return (exec_pwd());
	else if (cmd->exec.builtin == BUILTIN_EXPORT)
		return (exec_export(shell, cmd->args + 1));
	else if (cmd->exec.builtin == BUILTIN_ENV)
		return (exec_env(shell->env));
	else if (cmd->exec.builtin == BUILTIN_UNSET)
		return (exec_unset(shell, cmd->args + 1));
	else if (cmd->exec.builtin == BUILTIN_EXIT)
		return (exec_exit(cmd->args + 1));
	return (0);
}

/*
Returns 0 on success and 1 on error
*/

int	exec_pwd(void)
{
	char	*buf;

	buf = NULL;
	buf = getcwd(buf, 0);
	if (!buf)
		return (perror(MINI), 1);
	if (write(STDOUT_FILENO, buf, ft_strlen(buf)) == -1 || write(STDOUT_FILENO,
			"\n", 1) == -1)
		return (free(buf), 1);
	free(buf);
	return (0);
}

/*
Returns 0 on success, 1 on failure


*/

int	exec_cd(const char *path, t_shell *shell)
{
	t_env	*pwd;
	t_env	*old_pwd;

	if (chdir(path) == -1)
		return (perror(MINI), 1);
	pwd = env_lookup(shell->env, "PWD");
	if (!pwd || !pwd->value)
		return (0);
	old_pwd = env_lookup(shell->env, "OLDPWD");
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
This doesn't work with printf for echo -n to infile.
It needs to be fflush(force flushed) in this case to make it work.

printf() is part of C standard I/O (stdio), which uses buffers:
stdout is line-buffered when connected to a terminal
stdout is fully-buffered when connected to a file or pipe
Buffered data is only flushed:
On newline (\n) if line-buffered
When buffer is full
Or when you explicitly call fflush(stdout)

So must use write

returns 0 unless write error
*/

int	exec_echo(char **args)
{
	bool	nl;

	nl = true;
	if (!args || !*args)
		return (write(1, "\n", 1));
	if (ft_strcmp(*args, "-n") == 0)
	{
		nl = false;
		args++;
	}
	while (*args)
	{
		if (write(STDOUT_FILENO, *args, ft_strlen(*args)) == -1)
			return (1);
		if (!args[1])
			break ;
		if (write(STDOUT_FILENO, " ", 1) == -1)
			return (1);
		args++;
	}
	if (nl == true && write(STDOUT_FILENO, "\n", 1) == -1)
		return (1);
	return (0);
}
