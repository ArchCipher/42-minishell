/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:43 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 21:10:16 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	open_redir_file(t_token_type flag, char *file);

/*
DESCRIPTION:
	Sets up the redirections for the command.
	Returns 0 on success, 1 on error.

NOTE:
	For heredoc, redirs->file stores the delimiter word token and redirs->fd
	is read end of the a pipe created by process_heredoc().
	For other redirections, redirs->file stores the file name and redirs->fd
	is the file descriptor of the opened file.
	redirs->fd is set to -1 after closing, to avoid double close.
*/

int	setup_redirs(t_redir *redirs)
{
	if (!redirs)
		return (0);
	while (redirs)
	{
		if (redirs->flag == REDIR_IN || redirs->flag == REDIR_OUT
				|| redirs->flag == APPEND)
			redirs->fd = open_redir_file(redirs->flag, redirs->file);
		if (redirs->fd == -1)
			return (perr_msg(redirs->file, strerror(errno), NULL, false), 1);
		if (((redirs->flag == REDIR_IN || redirs->flag == HEREDOC)
				&& dup2(redirs->fd, STDIN_FILENO) == -1)
			|| ((redirs->flag == REDIR_OUT || redirs->flag == APPEND)
				&& dup2(redirs->fd, STDOUT_FILENO) == -1))
		{
			close(redirs->fd);
			redirs->fd = -1;
			return (perror(MINI), 1);
		}
		close(redirs->fd);
		redirs->fd = -1;
		redirs = redirs->next;
	}
	return (0);
}

/*
DESCRIPTION:
	Opens the redir file and returns the file descriptor.
	Returns valid fd on success, -1 if file cannot be opened.

NOTE:
	The last line, return (-1), should be unreachable.
*/
static int	open_redir_file(t_token_type flag, char *file)
{
	if (flag == REDIR_IN)
		return (open(file, O_RDONLY));
	else if (flag == REDIR_OUT)
		return (open(file, O_WRONLY | O_CREAT | O_TRUNC, 0660));
	else if (flag == APPEND)
		return (open(file, O_WRONLY | O_CREAT | O_APPEND, 0660));
	return (-1);
}

/*
DESCRIPTION:
	Closes the pipe fds and the previous fd if it exists.

NOTE:
	fd is the read and write end of the pipe.
	prev_fd is the read end of the previous command.
	Used in fork_with_pipe() and handle_heredoc().
*/
void	close_pipe_fds(int *fd, int prev_fd)
{
	if (fd)
	{
		close(fd[0]);
		close(fd[1]);
	}
	if (prev_fd != -1)
		close(prev_fd);
}

/*
DESCRIPTION:
	closes fds, prints error message and return 1.
*/

int	close_fds_error(int *fd, int prev_fd)
{
	close_pipe_fds(fd, prev_fd);
	perror(MINI);
	return (1);
}
