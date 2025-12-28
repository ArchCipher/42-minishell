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

static int	handle_heredoc(t_redir *redir); //, int status);
static int	exec_heredoc(char *limiter, int fd);
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
				shell->status = handle_heredoc(current); //, *status);
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

static int	handle_heredoc(t_redir *redir) //, int status) // should expand var
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
		ret = exec_heredoc(redir->file, fd[1]);
		close(fd[1]);
		exit(ret);
	}
	close(fd[1]);
	redir->fd = fd[0];
	return (heredoc_waitpid(pid));
}

/*
!!! should expand variables !!!
search for dollar. print until dollar. expand and then print again.

DESCRIPTION:
	Executes the heredoc and returns the status.
	It reads from terminal and writes to write end of a pipe,
		which is later read
	by command via stdin.

	!line means that it was interrupted by signal, so it returns 1 (error).
*/

// static int	expand_string(char *line, int status)
// {
// 	char	*var;
// 	char	exit_code[4];
// 	size_t	var_len;
//     char    *p;

//     if (!(p = ft_strchr(line, '$')))
//         return (line);
//     p++;
// 	var_len = 0;
// 	if (*p == '?')
// 	{
// 		p++;
// 		var = itoa_status(status, exit_code);
// 	}
// 	else
// 		var = expand_var(p, strlen(line) - (p - line));
// 	if (var == ERR)
// 		return (0);
// 	if (!var)
// 		return (1);
// 	var_len = ft_strlen(var);
// 	if (str->cap < str->i + var_len + (end - *token))
// 	{
// 		str->cap = str->i + var_len + (end - *token);
// 		str->str = ft_realloc(str->str, str->i, (str->cap) + 1);
// 		if (!str->str)
// 			return (perror(MINI), 0);
// 	}
// 	ft_memcpy(str->str + (str->i), var, var_len);
// 	str->i += var_len;
// 	return (1);
// }

static int	exec_heredoc(char *limiter, int fd)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (!line || !ft_strcmp(line, limiter))
			break ;
		if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
			return (free(line), 1);
		free(line);
	}
	if (!line)
		return (1);
	free(line);
	return (0);
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
