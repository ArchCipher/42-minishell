/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:45 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/07 18:29:23 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	is_builtin(char **s);
static int	exec_in_parent(t_cmd *cmd, t_shell *shell);
static int	restore_fds(int actual_stdin, int o_stdout, int o_stderr, int ret);
static int	cmds_waitpid(t_cmd **cmd);

/*
DESCRIPTION:
	Executes the commands and returns the status.
	It executes it in the parent process, if it is a single builtin command.
	In case of multiple commands, it forks a child process for each command
	and waits for them to finish.
	It returns the status of the last command.
*/

int	exec_cmds(t_cmd *cmd, t_shell *shell)
{
	t_cmd	*pipe_head;

	if (cmd->args && (!*cmd->args || !**cmd->args))
		return (perr_msg("", E_CMD, NULL, false), EXIT_CMD_NOT_FOUND);
	pipe_head = NULL;
	cmd->exec.builtin = is_builtin(cmd->args);
	if (cmd->exec.builtin != -1 && !cmd->next && !cmd->sub)
		return (exec_in_parent(cmd, shell));
	while (cmd)
	{
		if ((cmd->con == OR && !shell->status) || (cmd->con == AND
				&& shell->status))
			return (shell->status);
		cmd->exec.builtin = is_builtin(cmd->args);
		fork_with_pipe(cmd, shell);
		if (!pipe_head)
			pipe_head = cmd;
		if (pipe_head && (!cmd->next || cmd->next->con != PIPE_CHAR))
			shell->status = cmds_waitpid(&pipe_head);
		cmd = cmd->next;
	}
	return (shell->status);
}

/*
DESCRIPTION:
	Checks if the command is a builtin command.
	Returns the builtin command if it is, -1 otherwise.
*/

static int	is_builtin(char **s)
{
	if (!s || !*s)
		return (-1);
	if (!**s)
		return (EMPTY_STRING);
	if (ft_strcmp(*s, "echo") == 0)
		return (BUILTIN_ECHO);
	if (ft_strcmp(*s, "cd") == 0)
		return (BUILTIN_CD);
	if (ft_strcmp(*s, "pwd") == 0)
		return (BUILTIN_PWD);
	if (ft_strcmp(*s, "export") == 0)
		return (BUILTIN_EXPORT);
	if (ft_strcmp(*s, "unset") == 0)
		return (BUILTIN_UNSET);
	if (ft_strcmp(*s, "env") == 0)
		return (BUILTIN_ENV);
	if (ft_strcmp(*s, "exit") == 0)
		return (BUILTIN_EXIT);
	return (-1);
}

/*
DESCRIPTION:
	Executes builtin in parent and restores standard input and output
	fds after execution.
NOTE:
	On exit from shell, "exit\n" should be written to stdout, before exec_exit
	is called. exec_exit() error messages should be printed after "exit\n".
*/

static int	exec_in_parent(t_cmd *cmd, t_shell *shell)
{
	int	o_stdin;
	int	o_stdout;
	int	o_stderr;
	int	ret;

	o_stdout = dup(STDOUT_FILENO);
	if (o_stdout == -1)
		return (perror(MINI), 1);
	o_stdin = dup(STDIN_FILENO);
	if (o_stdin == -1)
		return (close(o_stdout), perror(MINI), 1);
	o_stderr = dup(STDERR_FILENO);
	if (o_stderr == -1)
		return (close(o_stdout), close(o_stdin), perror(MINI), 1);
	if (setup_redirs(cmd->redirs))
		return (restore_fds(o_stdin, o_stdout, o_stderr, 1));
	if (isatty(o_stdout) && cmd->exec.builtin == BUILTIN_EXIT)
		write(o_stdout, "exit\n", 5);
	ret = exec_builtin(cmd, shell);
	ret = restore_fds(o_stdin, o_stdout, o_stderr, ret);
	if (cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1]
			|| (cmd->args[1] && !cmd->args[2])
			|| (cmd->args[2] && errno == EINVAL)))
		exit_shell(ret, cmd, shell);
	return (ret);
}

/*
DESCRIPTION:
	Restores standard input and output fds after execution.
	Returns the exit status of the builtin execution or 1 on error.
*/

static int	restore_fds(int o_stdin, int o_stdout, int o_stderr, int ret)
{
	if ((dup2(o_stdin, STDIN_FILENO) == -1)
		|| (dup2(o_stdout, STDOUT_FILENO) == -1)
		|| (dup2(o_stderr, STDERR_FILENO) == -1))
	{
		perror(MINI);
		ret = 1;
	}
	close(o_stdin);
	close(o_stdout);
	close(o_stderr);
	return (ret);
}

/*
DESCRIPTION:
	Waits for the child processes to finish and returns the exit status of the
	last command.
	Returns 0 on success, 1 on error.

	waitpid(): Waits for a child process to update its status.
		WIFEXITED() returns true if the child process terminated normally.
		WIFSIGNALED() returns true if the child terminated due to a signal.
		WEXITSTATUS() returns the exit status of the child process.
		WTERMSIG() returns the signal that caused the child process to terminate.
	If waitpid() returns -1, WIFSIGNALED or WEXITSTATUS is not set.
*/

static int	cmds_waitpid(t_cmd **cmd)
{
	t_cmd	*cur;
	int		status;
	int		last_status;

	last_status = 0;
	cur = *cmd;
	while (cur && cur->exec.pid != -1)
	{
		if (waitpid(cur->exec.pid, &status, 0) != -1)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = SIG_EXIT_BASE + WTERMSIG(status);
		}
		else
		{
			perror(MINI);
			last_status = 1;
		}
		cur = cur->next;
	}
	*cmd = NULL;
	return (last_status);
}
