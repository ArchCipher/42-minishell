/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:05 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 20:28:25 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	handle_heredoc(t_redir *redir, t_shell *shell);
static int	exec_heredoc(char *limiter, int fd, bool quoted, t_shell *shell);
static int	expand_dollar(char **line, char *end, size_t *len, t_shell *shell);
static int	heredoc_waitpid(pid_t pid);

/*
DESCRIPTION:
	Handles the heredoc and sets the file descriptor to the read end of the pipe.

	SIGINT is ignored (SIG_IGN) during waitpid(). This prevents the parent from
	handling SIGINT during waitpid(). If the parent kept the handler,
		it would run
	while blocked in waitpid(), causing issues. With SIG_IGN, the signal still
	reaches the child (which has SIG_DFL), so the child dies.
	After waitpid() signal is handled,
		the parent process displays a new prompt on a new line.
*/

int	process_heredoc(t_cmd *cmds, t_shell *shell)
{
	t_redir	*current;
	t_cmd	*cmd;

	cmd = cmds;
	while (cmd)
	{
		current = cmd->redirs;
		while (current)
		{
			if (current->flag == heredoc)
			{
				shell->status = handle_heredoc(current, shell);
				if (shell->status == 1)
					return (1);
				if (shell->status == -1)
					exit_shell(1, cmds, shell);
			}
			current = current->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

/*
DESCRIPTION:
	Handles the heredoc and sets the file descriptor to the read end of the pipe.
	It creates a pipe and forks a child process.
	The chils reads from readline() and writes to the write end of the pipe.
	The parent waits for the child process to finish and returns the status.
	Returns 0 on success, 1 on heredoc error, -1 on signal error.
	Signal error should terminate the shell.
*/

static int	handle_heredoc(t_redir *redir, t_shell *shell)
{
	pid_t	pid;
	int		fd[2];
	int		ret;

	if (pipe(fd) == -1)
		return (perror(MINI), 1);
	pid = fork();
	if (pid == -1)
		return (perror(MINI), close_pipe_fds(fd, -1), 1);
	if (pid == 0)
	{
		if (setup_handler(SIGINT, SIG_DFL) == -1 || setup_handler(SIGQUIT,
				SIG_IGN) == -1)
		{
			close_pipe_fds(fd, -1);
			exit(1);
		}
		close(fd[0]);
		ret = exec_heredoc(redir->file, fd[1], redir->quoted, shell);
		close(fd[1]);
		exit(ret);
	}
	close(fd[1]);
	redir->fd = fd[0];
	return (heredoc_waitpid(pid));
}

/*
DESCRIPTION:
	Executes the heredoc and returns the status.
	It reads from terminal and writes to write end of a pipe,
		which is later read
	by command via stdin.
*/

static int	exec_heredoc(char *limiter, int fd, bool quoted, t_shell *shell)
{
	char	*line;
	size_t	len;

	line = readline("> ");
	while (line && ft_strcmp(line, limiter))
	{
		len = ft_strlen(line);
		if (!quoted && ft_memchr(line, '$', len) && !expand_dollar(&line, line + len, &len, shell))
			return (free(line), 1);
		if (write(fd, line, len) == -1 || write(fd, "\n", 1) == -1)
			return (free(line), 1);
		free(line);
		line = readline("> ");
	}
	free(line);
	return (0);
}

/*
expands variable if limiter was quoted
search for dollar. print until dollar. expand and then print again.
*/
static int	expand_dollar(char **line, char *end, size_t *len, t_shell *shell)
{
	t_string	str;
	char		*current;

	str.s = malloc(*len + 1);
	if (!str.s)
		return (0);
	str.cap = end - *line;
	str.len = 0;
	current = *line;
	while(*current)
	{
		if (dollar_expandable(current, end))
		{
			current++;
			if (!copy_var(&str, get_var(&current, end, shell), end - current))
				return (free(str.s), 0);
		}
		else
			str.s[str.len++] = *(current++);
	}
	str.s[str.len] = 0;
	free(*line);
	*line = str.s;
	*len = str.len;
	return (1);
}

/*
ctrl+D should not print newline.

handle setup handler errors
Returns 0 on success, 1 on heredoc error, -1 on signal error.
	Signal error should terminate the shell.
*/

static int	heredoc_waitpid(pid_t pid)
{
	int	status;
	int	ret;

	if (setup_handler(SIGINT, SIG_IGN) == -1)
		return (-1);
	ret = waitpid(pid, &status, 0);
	if (setup_handler(SIGINT, shell_handler) == -1)
		return (-1);
	if (ret == -1)
		return (perror(MINI), 1);
	if (WIFSIGNALED(status) || WEXITSTATUS(status))
	{
		write(1, "\n", 1);
		return (1);
	}
	return (0);
}
