#include "minishell.h"

/*
    if no infile, prints errror.
    for heredoc, redirs->file stores the delimiter and redirs->fd is read end of the a pipe created by:
        int process_heredoc(t_cmd *cmds, t_shell *shell);
*/

int setup_redirs(t_redir *redirs)
{
    if (!redirs)
        return (0);
    while(redirs)
    {
        if (redirs->flag == redir_in)
            redirs->fd = open(redirs->file, O_RDONLY);
        else if (redirs->flag == redir_out)
            redirs->fd = open(redirs->file, O_WRONLY | O_CREAT | O_TRUNC, 0660);
        else if (redirs->flag == append)
            redirs->fd = open(redirs->file, O_WRONLY | O_CREAT | O_APPEND, 0660);
        if (redirs->fd == -1)
            return (ft_dprintf(STDERR_FILENO, "%s: %s: %s\n", MINI, redirs->file, strerror(errno)), -1);
        if (((redirs->flag == redir_in || redirs->flag == heredoc) && dup2(redirs->fd, STDIN_FILENO) == -1) || ((redirs->flag == redir_out || redirs->flag == append) && dup2(redirs->fd, STDOUT_FILENO) == -1))
        {
            close(redirs->fd);
            perror(MINI);
            return (-1);
        }
        close(redirs->fd);
        redirs = redirs->next;
    }
    return (0);
}

void    close_pipe_fds(int *fd, int prev_fd)
{
    if (fd)
    {
        close(fd[0]);
        close(fd[1]);
    }
    if (prev_fd != -1)
        close(prev_fd);
}
