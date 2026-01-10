/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:05 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 16:00:57 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_heredoc(t_redir *redir, t_shell *shell);
static int	exec_heredoc(char *limiter, int fd, t_shell *shell);
static int	expand_dollar(char **line, size_t *len, t_shell *shell);
static int	wait_heredoc_child(pid_t pid);

/*
DESCRIPTION:
	Handles the heredoc using a pipe and stores the read end of the pipe in the
	redir struct. Returns 0 on success, 1 on error. It exits the shell if the
	signal couldn't be set for waitpid().

	while blocked in waitpid(), causing issues. With SIG_IGN, the signal still
	reaches the child (which has SIG_DFL), so the child dies.
	After waitpid() signal is handled,
		the parent process displays a new prompt on a new line.
*/

int	process_heredoc(t_list *cmds, t_shell *shell)
{
	t_list	*head;
	t_list	*redirs;

	head = cmds;
	while (cmds && cmds->content)
	{
		redirs = get_cmd(cmds)->redirs;
		while (redirs && redirs->content)
		{
			if (get_redir(redirs)->flag == HEREDOC)
			{
				shell->status = handle_heredoc(get_redir(redirs), shell);
				if (shell->status == 1)
					return (1);
				if (shell->status == -1)
				{
					shell->status = 1;
					exit_shell(head, shell);
				}
			}
			redirs = redirs->next;
		}
		cmds = cmds->next;
	}
	return (0);
}

/*
DESCRIPTION:
	It creates a pipe and forks a child process. The child process reads from
	readline() and writes to the write end of the pipe. The parent process
	waits for the child process to finish and returns the status.
	Returns 0 on success, -1 on signal error and 1 on other errors.
	Signal error should terminate the shell.
*/

static int	handle_heredoc(t_redir *redir, t_shell *shell)
{
	pid_t	pid;
	int		fd[2];

	if (pipe(fd) == -1)
		return (perror(MINI), 1);
	pid = fork();
	if (pid == -1)
		return (close_fds_error(fd, -1));
	if (pid == 0)
	{
		if (set_signal_handlers(SIG_DFL, SIG_IGN))
			exit(close_fds_error(fd, -1));
		close(fd[0]);
		exit(exec_heredoc(redir->file, fd[1], shell));
	}
	close(fd[1]);
	redir->fd = fd[0];
	return (wait_heredoc_child(pid));
}

/*
DESCRIPTION:
	Reads from terminal and writes to write end of a pipe, until the limiter is
	entered. It expands variables if the limiter was not quoted.
	It returns 0 on success, 1 on error.
*/

static int	exec_heredoc(char *limiter, int fd, t_shell *shell)
{
	char	*line;
	size_t	len;
	bool	quoted;

	quoted = (ft_strchr(limiter, '\'') || ft_strchr(limiter, '"'));
	remove_quotes(limiter, limiter);
	line = readline("> ");
	while (line && ft_strcmp(line, limiter))
	{
		shell->line_num++;
		len = ft_strlen(line);
		if (!quoted && ft_memchr(line, '$', len)
			&& expand_dollar(&line, &len, shell))
			return (free(line), close(fd), 1);
		if (write(fd, line, len) == -1 || write(fd, "\n", 1) == -1)
			return (perror(MINI), free(line), close(fd), 1);
		free(line);
		line = readline("> ");
	}
	if (!line)
		ft_dprintf(STDERR_FILENO, "%s: %s %d %s (wanted `%s')\n", MINI, W_EOF1,
			shell->line_num, W_EOF2, limiter);
	free(line);
	close(fd);
	return (0);
}

/*
DESCRIPTION:
	Creates a string from the line and expands variables if the line contains a
	dollar sign. Returns 0 on success, 1 on error.
*/

static int	expand_dollar(char **src, size_t *len, t_shell *shell)
{
	t_expand	s;

	s = init_expand(*src, *len);
	if (!s.dst)
		return (perror(MINI), 1);
	while (s.src < s.src_end)
	{
		if (!dollar_expandable(s.src, s.src_end))
			s.dst[s.len++] = *(s.src++);
		else if (append_var(&s, shell))
			return (free(s.dst), 1);
	}
	s.dst[s.len] = 0;
	free(*src);
	*src = s.dst;
	*len = s.len;
	return (0);
}

/*
DESCRIPTION:
	Waits for the child process to finish and returns the status.
	It sets the signal handler for SIGINT to ignored (SIG_IGN) during waitpid().
	This prevents the parent from handling SIGINT during waitpid(). If the parent
	kept the handler, it would run the handler while blocked in waitpid(),
	causing issues.
	It returns 0 on success, 1 on heredoc error, -1 on signal error.
	Signal error should terminate the shell.

NOTE:
	It is the readline() that prints newline on end of file (Ctrl+D), not
	heredoc_waitpid(). If waitpid() returns -1, WIFSIGNALED or WEXITSTATUS is
	not set.
*/

static int	wait_heredoc_child(pid_t pid)
{
	int	status;
	int	ret;

	if (set_signal_handler(SIGINT, SIG_IGN) == -1)
		return (perror(MINI), -1);
	ret = waitpid(pid, &status, 0);
	if (set_signal_handler(SIGINT, shell_handler) == -1)
		return (perror(MINI), -1);
	if (ret == -1)
		return (perror(MINI), 1);
	if (WIFSIGNALED(status))
	{
		write(1, "\n", 1);
		g_signal = WTERMSIG(status);
		return (1);
	}
	return (WIFEXITED(status) && WEXITSTATUS(status));
}
