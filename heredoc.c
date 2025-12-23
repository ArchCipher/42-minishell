#include "minishell.h"

static int  handle_heredoc(t_redir *redir);
static int  exec_heredoc(char *limiter, int fd);
static int  heredoc_waitpid(pid_t pid);
static void exit_shell(t_cmd *cmds, t_shell *shell);

/*
DESCRIPTION:
    Handles the heredoc and sets the file descriptor to the read end of the pipe.

    SIGINT is ignored (SIG_IGN) during waitpid(). This prevents the parent from
    handling SIGINT during waitpid(). If the parent kept the handler, it would run
    while blocked in waitpid(), causing issues. With SIG_IGN, the signal still
    reaches the child (which has SIG_DFL), so the child dies.
    After waitpid() signal is handled, the parent process displays a new prompt on a new line.
*/

int process_heredoc(t_cmd *cmds, t_shell *shell)
{
    t_redir *current;
    t_cmd   *cmd;
    int     ret;

    cmd = cmds;
    while(cmd)
    {
        current = cmd->redirs;
        while(current)
        {
            if (current->flag == heredoc)
            {
                ret = handle_heredoc(current);
                if (ret == 1)
                    return (shell->status = 1);
                else if (ret == -1)
                    exit_shell(cmds, shell);
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
    The child process reads from the readline() and writes to the write end of the pipe.
    The parent process waits for the child process to finish and returns the status.
    Returns 0 on success, 1 on heredoc error, -1 on signal error.
    Signal error should terminate the shell.
*/

static int  handle_heredoc(t_redir *redir)
{
    int     fd[2];
    pid_t   pid;
    int     ret;

    if (pipe(fd) == -1)
        return (perror(MINI), 1);
    pid = fork();
    if (pid == -1)
        return (perror(MINI), close_pipe_fds(fd, -1), 1);
    if (pid == 0)
    {
        if (setup_handler(SIGINT, SIG_DFL) == -1 || setup_handler(SIGQUIT, SIG_IGN) == -1)
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
!!! should expland variables !!!

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
    if (line)
        free(line);
    return (0);
}

/*
ctrl+D should not print newline.

handle setup handler errors
Returns 0 on success, 1 on heredoc error, -1 on signal error.
    Signal error should terminate the shell.
*/

static int heredoc_waitpid(pid_t pid)
{
    int status;
    int ret;

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

// exits in heredoc should cleanup...
// garge collector + exit


static void exit_shell(t_cmd *cmds, t_shell *shell)
{
    free_cmds(cmds);
    tcsetattr(STDIN_FILENO, TCSANOW, &shell->original_term);
    rl_clear_history();
    exit(EXIT_FAILURE);
}
