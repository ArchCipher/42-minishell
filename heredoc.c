#include "minishell.h"

static int  handle_heredoc(t_redir *redir);
static int  exec_heredoc(char *limiter, int fd);
static int  heredoc_waitpid(pid_t pid);
static int  heredoc_fork_error(int *fd);


/*
DESCRIPTION:
    Handles the heredoc and sets the file descriptor to the read end of the pipe.

    SIGINT is ignored (SIG_IGN) during waitpid(). This prevents the parent from
    handling SIGINT during waitpid(). If the parent kept the handler, it would run
    while blocked in waitpid(), causing issues. With SIG_IGN, the signal still
    reaches the child (which has SIG_DFL), so the child dies.
    After waitpid() signal is handled, the parent process displays a new prompt on a new line.
*/

int process_heredoc(t_cmd *cmds, int *status)
{
    t_redir *current;

    while(cmds)
    {
        current = cmds->redirs;
        while(current)
        {
            if (current->flag == heredoc && handle_heredoc(current))
                return (*status = 1);
            current = current->next;
        }
        cmds = cmds->next;
    }
    return (0);
}

/*
DESCRIPTION:
    Handles the heredoc and sets the file descriptor to the read end of the pipe.
    It creates a pipe and forks a child process.
    The child process reads from the readline() and writes to the write end of the pipe.
    The parent process waits for the child process to finish and returns the status.
    It returns 1 if there is an error, 0 if successful.
*/

static int  handle_heredoc(t_redir *redir)
{
    int     fd[2];
    pid_t   pid;

    if (pipe(fd) == -1)
        return (perror(MINI), 1);
    pid = fork();
    if (pid == -1)
        return (heredoc_fork_error(fd));
    if (pid == 0)
    {
        setup_handler(SIGINT, SIG_DFL);
        setup_handler(SIGQUIT, SIG_IGN);
        close(fd[0]);
        exit (exec_heredoc(redir->file, fd[1]));
    }
    close(fd[1]);
    if (heredoc_waitpid(pid))
        return (close(fd[0]), 1);
    redir->fd = fd[0];
    return (0);
}

/*
DESCRIPTION:
    Executes the heredoc and returns the status.
    It reads from terminal and writes to write end of a pipe and later read by command via stdin.
*/

static int  exec_heredoc(char *limiter, int fd)
{
    char *line;

    while(1)
    {
        line = readline("> ");
        if (!line || !ft_strcmp(line, limiter))
            break;
        if (write(fd, line, ft_strlen(line)) == -1 || write(fd, "\n", 1) == -1)
            return (free(line), 1);
        free(line);
    }
    return (0);
}

static int heredoc_waitpid(pid_t pid)
{
    int status;

    setup_handler(SIGINT, SIG_IGN);
    waitpid(pid, &status, 0);
    setup_handler(SIGINT, shell_handler);
    if (WIFSIGNALED(status) || WEXITSTATUS(status))
        return (write(1, "\n", 1), 1);
    return (0);  
}

static int heredoc_fork_error(int *fd)
{
    close(fd[0]);
    close(fd[1]);
    perror(MINI);
    return (1);
}
