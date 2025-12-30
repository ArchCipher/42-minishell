/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:45 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:53:32 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
heredoc
execve wrapper
make sure errors are not printed twice
*/
static int	is_builtin(char *s);
static int	exec_in_parent(t_cmd *cmd, t_shell *shell);
static int	restore_fds(int actual_stdin, int actual_stdout, int ret);
static int	cmds_waitpid(t_cmd *cmds);

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
returns -1 if not a builtin
*/

static int	is_builtin(char *s)
{
	if (ft_strcmp(s, "echo") == 0)
		return (BUILTIN_ECHO);
	else if (ft_strcmp(s, "cd") == 0)
		return (BUILTIN_CD);
	else if (ft_strcmp(s, "pwd") == 0)
		return (BUILTIN_PWD);
	else if (ft_strcmp(s, "export") == 0)
		return (BUILTIN_EXPORT);
	else if (ft_strcmp(s, "unset") == 0)
		return (BUILTIN_UNSET);
	else if (ft_strcmp(s, "env") == 0)
		return (BUILTIN_ENV);
	else if (ft_strcmp(s, "exit") == 0)
		return (BUILTIN_EXIT);
	return (-1);
}

/*
DESCRIPTION:
	stores actual stdin and stdout and executes builtin in parent and restores
    stdin and stdout after execution.
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
	if (setup_redirs(cmd->redirs) == -1)
		return (restore_fds(actual_stdin, actual_stdout, 1));
	if (isatty(actual_stdout) && cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1] || (cmd->args[1]
				&& !cmd->args[2])))
		write(actual_stdout, "exit\n", 5);
	ret = exec_builtin(cmd, shell);	
	if (cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1] || (cmd->args[1]
				&& !cmd->args[2])))
	{
		close(actual_stdin);
		close(actual_stdout);
		exit_shell(ret, cmd, shell);
	}
	return (restore_fds(actual_stdin, actual_stdout, ret));
}

static int	restore_fds(int actual_stdin, int actual_stdout, int ret)
{
	if ((dup2(actual_stdout, STDOUT_FILENO) == -1) || (dup2(actual_stdin, STDIN_FILENO) == -1))
	{
		perror(MINI);
		ret = 1;
	}
	close(actual_stdin);
	close(actual_stdout);
	return (ret);
}

/*
WAITPID:
	to prevent zombie processes, the below code was not used,
	as this can return early if one of the child processes fail.
	if (waitpid(cmd->pid, &status, 0) == -1)
		return (-1); // handle error
*/

static int	cmds_waitpid(t_cmd *cmds)
{
	int	status;
	int	last_status;

	last_status = 0;
	while (cmds)
	{
		if (cmds->exec.pid == -1)
			break ;
		if (waitpid(cmds->exec.pid, &status, 0) != -1)
		{
			if (WIFEXITED(status))
				last_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				last_status = SIG_EXIT_BASE + WTERMSIG(status);
		}
		else
			perror(MINI);
		cmds = cmds->next;
	}
	return (last_status);
}
