/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:45 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 14:16:01 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
make sure errors are not printed twice
*/
static int	is_builtin(char *s);
static int	exec_in_parent(t_cmd *cmd, t_shell *shell);
static int	restore_fds(int actual_stdin, int actual_stdout, int ret);
static int	cmds_waitpid(t_cmd *cmds);

/*
DESCRIPTION:
	Executes the commands and returns the status.
	It executes it in the parent process, if it is a single builtin command.
	In case of multiple commands, it forks a child process for each command
	and waits for them to finish.
	It returns the status of the last command.
*/

int	exec_cmds(t_cmd *cmds, t_shell *shell)
{
	t_cmd	*cmd;
	int		prev_fd;

	cmd = cmds;
	prev_fd = -1;
	cmd->exec.builtin = is_builtin(cmd->args[0]);
	if (cmd->exec.builtin != -1 && !cmd->next)
		return (exec_in_parent(cmd, shell));
	while (cmd)
	{
		cmd->exec.builtin = is_builtin(cmd->args[0]);
		if (fork_with_pipe(cmd, &prev_fd, shell))
			break ;
		cmd = cmd->next;
	}
	return (cmds_waitpid(cmds));
}

/*
DESCRIPTION:
	Checks if the command is a builtin command.
	Returns the builtin command if it is, -1 otherwise.
*/

static int	is_builtin(char *s)
{
	if (ft_strcmp(s, "echo") == 0)
		return (BUILTIN_ECHO);
	if (ft_strcmp(s, "cd") == 0)
		return (BUILTIN_CD);
	if (ft_strcmp(s, "pwd") == 0)
		return (BUILTIN_PWD);
	if (ft_strcmp(s, "export") == 0)
		return (BUILTIN_EXPORT);
	if (ft_strcmp(s, "unset") == 0)
		return (BUILTIN_UNSET);
	if (ft_strcmp(s, "env") == 0)
		return (BUILTIN_ENV);
	if (ft_strcmp(s, "exit") == 0)
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
	int	actual_stdin;
	int	actual_stdout;
	int	ret;

	actual_stdout = dup(STDOUT_FILENO);
	if (actual_stdout == -1)
		return (perror(MINI), 1);
	actual_stdin = dup(STDIN_FILENO);
	if (actual_stdin == -1)
		return (close(actual_stdout), perror(MINI), 1);
	if (setup_redirs(cmd->redirs))
		return (restore_fds(actual_stdin, actual_stdout, 1));
	if (isatty(actual_stdout) && cmd->exec.builtin == BUILTIN_EXIT
		&& (!cmd->args[1] || (cmd->args[1] && !cmd->args[2])))
		write(actual_stdout, "exit\n", 5);
	ret = exec_builtin(cmd, shell);
	ret = restore_fds(actual_stdin, actual_stdout, ret);
	if (cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1] || (cmd->args[1]
				&& !cmd->args[2])))
		exit_shell(ret, cmd, shell);
	return (ret);
}

/*
DESCRIPTION:
	Restores standard input and output fds after execution.
	Returns the exit status of the builtin execution or 1 on error.
*/

static int	restore_fds(int actual_stdin, int actual_stdout, int ret)
{
	if ((dup2(actual_stdin, STDIN_FILENO) == -1) || (dup2(actual_stdout,
				STDOUT_FILENO) == -1))
	{
		perror(MINI);
		ret = 1;
	}
	close(actual_stdin);
	close(actual_stdout);
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

static int	cmds_waitpid(t_cmd *cmds)
{
	int	status;
	int	last_status;

	last_status = 0;
	while (cmds && cmds->exec.pid != -1)
	{
		if (waitpid(cmds->exec.pid, &status, 0) != -1)
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
		cmds = cmds->next;
	}
	return (last_status);
}
