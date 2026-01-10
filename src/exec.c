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

static int	exec_cmd(t_list *cmds, t_shell *shell);
static int	is_builtin(char **s);
static int	cmds_waitpid(t_list **pipe, t_shell *shell);

/*
DESCRIPTION:
	Executes the commands and returns the status.
	It executes it in the parent process, if it is a single builtin command.
	In case of multiple commands, it forks a child process for each command
	and waits for them to finish.
	It returns the status of the last command.
*/

int	exec_cmds(t_list *cmds, t_shell *shell)
{
	t_list	*pipe;
	t_cmd	*cmd;

	pipe = NULL;
	while (cmds && cmds->content)
	{
		cmd = get_cmd(cmds);
		if ((cmd->con == OR && !shell->status) || (cmd->con == AND
				&& shell->status))
			return (shell->status);
		shell->status = exec_cmd(cmds, shell);
		if (!pipe)
			pipe = cmds;
		if (pipe && (!cmds->next || !cmds->next->content
				|| get_cmd(cmds->next)->con != PIPE_CHAR)
			&& cmds_waitpid(&pipe, shell) == -1)
			exit_shell(cmds, shell);
		cmds = cmds->next;
	}
	return (shell->status);
}

static int	exec_cmd(t_list *cmds, t_shell *shell)
{
	t_cmd	*cmd;

	cmd = get_cmd(cmds);
	if (!cmd || (cmd->args && expand_args(cmd, shell)))
		return (1);
	if (cmd->args && (!*cmd->args || !**cmd->args))
	{
		perr_msg("", E_CMD, NULL, false);
		return (EXIT_CMD_NOT_FOUND);
	}
	cmd->exec.builtin = is_builtin(cmd->args);
	if (cmd->exec.builtin == -1 || cmd->subshell
		|| (cmds->next && get_cmd(cmds->next)->con == PIPE_CHAR))
		return (fork_with_pipe(cmds, shell));
	return (exec_in_parent(cmds, shell));
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
	Waits for the child processes to finish and returns the exit status of the
	last command.
	Returns the exit status of the last command on success, -1 on error.

	waitpid(): Waits for a child process to update its status.
		WIFEXITED() returns true if the child process terminated normally.
		WIFSIGNALED() returns true if the child terminated due to a signal.
		WEXITSTATUS() returns the exit status of the child process.
		WTERMSIG() returns the signal that caused the child process to terminate.
	If waitpid() returns -1, WIFSIGNALED or WEXITSTATUS is not set.
*/

static int	cmds_waitpid(t_list **pipe, t_shell *shell)
{
	int		status;

	if (set_signal_handler(SIGINT, SIG_IGN) == -1)
		return (perror(MINI), -1);
	while (*pipe && (*pipe)->content && get_cmd(*pipe)->exec.pid != -1)
	{
		if (waitpid(get_cmd(*pipe)->exec.pid, &status, 0) != -1)
		{
			if (WIFEXITED(status))
				shell->status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				shell->status = SIG_EXIT_BASE + WTERMSIG(status);
		}
		else
		{
			perror(MINI);
			shell->status = 1;
		}
		*pipe = (*pipe)->next;
	}
	if (set_signal_handler(SIGINT, shell_handler) == -1)
		return (perror(MINI), -1);
	*pipe = NULL;
	return (shell->status);
}
