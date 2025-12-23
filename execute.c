#include "minishell.h"

/*
heredoc
execve wrapper
make sure errors are not printed twice
*/
static int is_builtin(char *s);
static int exec_in_parent(t_cmd *cmd);
static int fork_with_pipe(t_cmd *cmd, char **envp, int *prev_fd);
static int cmds_waitpid(t_cmd *cmds);

int exec_cmds(t_cmd *cmds, char **envp)
{
    t_cmd   *cmd;
    int     prev_fd;

    cmd = cmds;
    prev_fd = -1;
    cmd->exec.builtin = is_builtin(cmd->args[0]);
    if (cmd->exec.builtin != -1 && !cmd->next)
        return (exec_in_parent(cmd));
    while(cmd)
    {
        cmd->exec.builtin = is_builtin(cmd->args[0]);
        if (fork_with_pipe(cmd, envp, &prev_fd))
            break ;
        cmd = cmd->next;
    }
    return (cmds_waitpid(cmds));
}

/*
returns -1 if not a builtin
*/

int is_builtin(char *s)
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
    else if(ft_strcmp(s, "exit") == 0)
        return (BUILTIN_EXIT);
    return (-1);
}

/*
DESCRIPTION:
    stores actual stdin and stdout and executes builtin in parent and restores stdin and stdout after execution.
*/

int exec_in_parent(t_cmd *cmd)
{
    int actual_stdout;
    int actual_stdin;
    int ret;

    if ((actual_stdout = dup(STDOUT_FILENO)) == -1)
        return (perror(MINI), 1);
    if ((actual_stdin = dup(STDIN_FILENO)) == -1)
        return (close(actual_stdout), perror(MINI), 1);
    if (setup_redirs(cmd->redirs) == -1)
        return (close(actual_stdout), close(actual_stdin), 1);
    if (cmd->exec.builtin == BUILTIN_EXIT)
        write(actual_stdout, "exit\n", 5);
    ret = exec_builtin(cmd);
    if (dup2(actual_stdout, STDOUT_FILENO) == -1)
        return (close(actual_stdout), close(actual_stdin), perror(MINI), 1);
    if (dup2(actual_stdin, STDIN_FILENO) == -1)
        return (close(actual_stdout), close(actual_stdin), perror(MINI), 1);
    close(actual_stdout);
    close(actual_stdin);
    return (ret);
}

/*
DESCRIPTION:
    If forks a child preocess, and if there is a next command, it creates a pipe and then forks.
    If fork succeeds, it executes the child, closes fds appropriately and stores the
    read_end of pipe for next command if there is a next command.
    Returns 0 on success and 1 on pipe or fork error.

    PIPE: a pipe with 2 fds with read and write ends are created.
        fd[0] connects to read end, fd[1] to write end.
    FORK: On success, a child process is created. It returns 0 to the child process and
        the process ID of the child to the parent process.
    On error, both pipe and fork return -1.
*/

int fork_with_pipe(t_cmd *cmd, char **envp, int *prev_fd)
{
    int fd[2];
    int *pipe_fd;

    pipe_fd = NULL;
    if (cmd->next && pipe(fd) == -1)
        return (perror(MINI), 1);
    if (cmd->next)
        pipe_fd = fd;
    cmd->exec.pid = fork();
    if (cmd->exec.pid == -1)
    {
        close_pipe_fds(pipe_fd, *prev_fd);
        return (perror(MINI), 1);
    }
    if (cmd->exec.pid == 0)
        exec_child(cmd, pipe_fd, *prev_fd, envp);
    if (*prev_fd != -1)
        close(*prev_fd);
    if (pipe_fd)
    {
        close(pipe_fd[1]);
        *prev_fd = pipe_fd[0];
    }
    return (0);
}

/*
WAITPID:
    to prevent zombie processes, the below code was not used,
    as this can return early if one of the child processes fail.
    if (waitpid(cmd->pid, &status, 0) == -1)
        return (-1); // handle error
*/

int cmds_waitpid(t_cmd *cmds)
{
    int status;
    int last_status;

    last_status = 0;
    while(cmds)
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
