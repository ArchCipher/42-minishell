/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirs.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:44:43 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/06 20:19:38 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	get_target_fd(t_redir *redir);
static int	open_redir_file(const t_redir *redir);

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

int	setup_redirs(t_list *redirs)
{
	t_redir	*redir;

	while (redirs && redirs->content)
	{
		redir = get_redir(redirs);
		redir->fd = open_redir_file(redir);
		if (redir->fd == -1)
			return (perr_msg(redir->file, strerror(errno), NULL, false), 1);
		get_target_fd(redir);
		if (dup2(redir->fd, redir->target_fd) == -1)
		{
			close(redir->fd);
			redir->fd = -1;
			return (perror(MINI), 1);
		}
		close(redir->fd);
		redir->fd = -1;
		redirs = redirs->next;
	}
	return (0);
}

static void	get_target_fd(t_redir *redir)
{
	if (redir->target_fd != -1)
		return ;
	if (redir->flag == REDIR_IN || redir->flag == HEREDOC)
		redir->target_fd = STDIN_FILENO;
	else if (redir->flag == REDIR_OUT || redir->flag == APPEND)
		redir->target_fd = STDOUT_FILENO;
}

/*
DESCRIPTION:
	Opens the redir file and returns the file descriptor.
	Returns valid fd on success, -1 if file cannot be opened.

NOTE:
	The last line, return (-1), should be unreachable.
*/

static int	open_redir_file(const t_redir *redir)
{
	if (redir->flag == HEREDOC)
		return (redir->fd);
	if (redir->flag == REDIR_IN)
		return (open(redir->file, O_RDONLY));
	if (redir->flag == REDIR_OUT)
		return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0660));
	if (redir->flag == APPEND)
		return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0660));
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
	Closes file descriptors, prints error message, and returns 1.
	Used for error cleanup in fork_with_pipe().
*/

int	close_fds_error(int *fd, int prev_fd)
{
	close_pipe_fds(fd, prev_fd);
	perror(MINI);
	return (1);
}
