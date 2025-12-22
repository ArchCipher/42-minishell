#include "minishell.h"

/*
heredoc
execve wrapper
make sure errors are not printed twice
*/

int exec_builtin(t_cmd *cmd)
{
    if (cmd->exec.builtin == BUILTIN_ECHO)
        return (exec_echo(cmd->args + 1));
    else if (cmd->exec.builtin == BUILTIN_CD)
        return (exec_cd(cmd->args[1]));
    else if (cmd->exec.builtin == BUILTIN_PWD)
        return (exec_pwd());
    // no skeleton yet
    // else if (cmd->exec.builtin == BUILTIN_EXPORT)
    //     exec_export();
    // else if (cmd->exec.builtin == BUILTIN_UNSET)
    //     exec_unset();
    // else if (cmd->exec.builtin == BUILTIN_ENV)
    //     exec_env();
    else if (cmd->exec.builtin == BUILTIN_EXIT)
        return (exec_exit(cmd->args[1]), 0);
    return (0);
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

void    child_path_error(char *filename, char *msg, int exit_code, char *path)
{
    printf("%s: %s: %s\n", MINI, filename, msg);
    if (!ft_strchr(filename, '/') && path)
        free(path);
    exit(exit_code); 
}

/*
DESCRIPTION
    Retrieves $PATH using getenv, splits by : using ft_strtok_r.
    For each directory in $PATH, builds dir + "/" + cmd.

    When path is empty "", it means current directory-> create full string with ""./" such as " "./cmd"

    This is child only logic and doesn't return invalid path. It exits on error.
    Caution: !!!cannot be used in parent process!!!
*/
char    *build_path(char *filename)
{
    t_path_vars vars;
    struct stat sb;

    if (!(vars.path = getenv("PATH")) || !(vars.full_path = ft_strdup(vars.path)))
    {
        perror(MINI);
        exit(1);
    }
    vars.path = ft_strtok_r(vars.full_path, ":", &vars.p);
    while(vars.path)
    {
        if (!*(vars.path))
            vars.new_path = ft_strjoin("./", filename);
        else
            vars.new_path = ft_strjoin3(vars.path, "/", filename);
        if (!vars.new_path)
        {
            free(vars.full_path);
            perror(MINI);
            exit(1);
        }
        if(stat(vars.new_path, &sb) == 0)
            break ;
        free(vars.new_path);
        vars.new_path = NULL;
        vars.path = ft_strtok_r(NULL, ":", &vars.p);
    }
    free(vars.full_path);
    return (vars.new_path);
}

/*
0:  success
1:  general error
126:    found but not executable (insufficient permissions)
127:    command not found

normal exit codes: 0-127
128 + N:    fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)
255:    exit status out of range (exit takes only integer args in the range 0 - 255)
non numeric argument for exit also return 255

If buils_path returns NULL path, or if stat(path) fails, it exits with code 127(command not found)
It checks S_ISREG(sb.st_mode) to check if it is a directory.
It checks access(path, X_OK) for execute permissions.
*/

char    *get_valid_path(char *filename)
{
    struct stat sb;
    char        *path;

    if (ft_strchr(filename, '/'))
        path = filename;
    else
        path = build_path(filename);
    if (!path)
        child_path_error(filename, E_PATH, EXIT_CMD_NOT_FOUND, path);
    if (stat(path, &sb) == -1)
        child_path_error(filename, strerror(errno), EXIT_CMD_NOT_FOUND, path);
    if (!S_ISREG(sb.st_mode) || !(access(path, X_OK) == 0))
        child_path_error(filename, strerror(errno), EXIT_CANNOT_EXEC, path);
    return (path);
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

/*
    if no infile, prints errror.
    for heredoc, redirs->file stores the delimiter and redirs->fd is read end of the a pipe created by:
        int process_heredoc(t_cmd *cmds, int *status)
*/

int setup_redirs(t_redir *redirs)
{
    if (!redirs)
        return (0);
    while(redirs)
    {
        if (redirs->flag == redir_in )
            redirs->fd = open(redirs->file, O_RDONLY);
        else if (redirs->flag == redir_out)
            redirs->fd = open(redirs->file, O_WRONLY | O_CREAT | O_TRUNC, 0660);
        else if (redirs->flag == append)
            redirs->fd = open(redirs->file, O_WRONLY | O_CREAT | O_APPEND, 0660);
        if (redirs->fd == -1)
            return (perror(MINI), -1);
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

// separate execve wrapper for built-in and executables

/*
DESCRIPTION:
    closes fds, prints error message and exits child on error.
*/

void    child_dup_error(int *fd, int prev_fd)
{
    close_pipe_fds(fd, prev_fd);
    perror(MINI);
    exit(1);
}

/*
DESCRIPTION:
    It sets up signals to default and duplicates required fds adn closes all fds.
    It then builds path if necessary and executes the command. If execve returns, cleans up and prints error.

    EXECVE:
    When a program is executed as a result of an execve() call, it is entered as follows:
        int main(int argc, char **argv, char **envp);
    As the execve() function overlays the current process image  with a new process image,
    the successful call has no process to return to.
    If execve() does return to the calling process, an error has occurred;
    the return value will be -1 and the global variable errno is set to indicate the error.
*/

void    exec_child(t_cmd *cmd, int *fd, int prev_fd, char **envp)
{
    char *path;

    setup_handler(SIGINT, SIG_DFL);
    setup_handler(SIGQUIT, SIG_DFL);
    if (fd && (dup2(fd[1], STDOUT_FILENO) == -1))
        child_dup_error(fd, prev_fd);
    if (prev_fd != -1 && (dup2(prev_fd, STDIN_FILENO) == -1))
        child_dup_error(fd, prev_fd);
    close_pipe_fds(fd, prev_fd);
    if (setup_redirs(cmd->redirs) == -1)
        exit(1);
    path = cmd->args[0];
    if (cmd->exec.builtin != -1)
        exit(exec_builtin(cmd));
    path = get_valid_path(cmd->args[0]);
    execve(path, cmd->args, envp);
    if (!ft_strchr(cmd->args[0], '/'))
        free(path);
    perror(MINI);
    exit(1);
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
        waitpid(cmds->exec.pid, &status, 0);
        if (WIFEXITED(status))
            last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
            last_status = SIG_EXIT_BASE + WTERMSIG(status);
        cmds = cmds->next;
    }
    return (last_status);
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
