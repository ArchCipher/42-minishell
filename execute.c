#include "minishell.h"

/*
handle redirections < > << >>
implement redirection operator logic
execve wrapper
*/

/*
POSIX exit codes:
127 → command not found
126 → found but not executable (insufficient permissions)
1   → general error
0   → success
128 + N → fatal error signal N (ex: 137 is 128 + 9 for SIGKILL)

If buils_path returns NULL path, or if stat(path) fails, it exits with code 127(command not found)
It hecks S_ISREG(sb.st_mode) to check if it is a directory.
It checks access(path, X_OK) for execute permissions.
*/

void    check_path_validity(char *path, bool free_path)
{
    struct stat sb;

    if (!path || stat(path, &sb) == -1)
        exit(127);
    if (!S_ISREG(sb.st_mode) || !(access(path, X_OK) == 0))
    {
        if (free_path)
            free(path);
        exit(126);  // directory or insufficient permissions
    }
}

/*
DESCRIPTION
    Retrieves $PATH using getenv, splits by : using ft_strtok_r.
    For each directory in $PATH, builds dir + "/" + cmd.

    When path is empty "", it means current directory-> create full string with ""./" such as " "./cmd"

    This is hild only logic and doesn't return invalid path. It exits on error.
    Caution: !!!cannot be used in parent process!!!
*/
char    *build_path(char *filename)
{
    char    *path;
    char    *full_path;
    char    *p;
    char    *new_path;
    struct stat sb;

    // maybe use strndup instead as this is only place i use strdup()
    if (!(path = getenv("PATH")) || !(full_path = ft_strdup(path))) 
        return (NULL);
    path = ft_strtok_r(full_path, ":", &p);
    while(path)
    {
        new_path = ft_strjoin3(path, "/", filename);
        if (!new_path)
            return (free(full_path), NULL);
        if(stat(new_path, &sb) == 0)
            break ;
        free(new_path);
        new_path = NULL;
        path = ft_strtok_r(NULL, ":", &p);
    }
    free(full_path);
    check_path_validity(new_path, true);
    return (new_path);
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
FROM EXECVE() MAN PAGE
int execve(const char *path, char *const argv[], char *const envp[]);
When a program is executed as a result of an execve() call, it is entered as follows:
    int main(int argc, char **argv, char **envp);

As the execve() function overlays the current process image  with a new process image,
the successful call has no process to return to.
If execve() does return to the calling process, an error has occurred;
the return value will be -1 and the global variable errno is set to indicate the error.
*/

int exec_fork(char **args, int *fd, int prev_fd, char **envp)
{
    pid_t   pid;
    char    *path;

    pid = fork();
    if (pid == -1)
    {
        close_pipe_fds(fd, prev_fd);
        return (-1); // handle error (close pipe fds if any)
    }
    if (pid == 0)
    {
        if (fd && (dup2(fd[1], STDOUT_FILENO) == - 1))
        {
            close_pipe_fds(fd, prev_fd);
            exit(-1); // handle_error
        }
        if (fd)
            close_pipe_fds(fd, -1);
        if (prev_fd != -1 && (dup2(prev_fd, STDIN_FILENO) == -1))
        {
            close(prev_fd);
            exit(-1);
        }
        if (prev_fd != -1)
            close(prev_fd);
        path = args[0];
        if (!ft_strchr(args[0], '/'))
            path = build_path(args[0]);
        else
            check_path_validity(path, false);
        execve(path, args, envp); // should never return
        // if returned
        if (!ft_strchr(args[0], '/'))
            free(path);
        perror("minishell: ");
        exit(1);
    }
    return (pid);
}

// TO DO: split as below for shorter fucntions
// forking
// piping
// waiting

/*
PIPE: 
// fd[2] first fd i.e., fd[0] connects to read end and second i.e., fd[1] to write end.
// pipe returns 0 on succes, -1 on error & errno is set.
WAITPID:
    to prevent zombie processes, the below code was not used,
    as this can return early if one of the child processes fail.
    if (waitpid(cmd->pid, &status, 0) == -1)
        return (-1); // handle error
*/

// separate execve wrapper for built-in and executables
int exec_cmds(t_cmd *cmds, char **envp, int *status)
{
    int     fd[2];
    int     prev_pipe;
    t_cmd   *cmd;

    cmd = cmds;
    prev_pipe = -1;
    while(cmd)
    {
        if (cmd->next)
        {
            if (pipe(fd) == -1)
                return (-1); // handle error
            cmd->pid = exec_fork(cmd->args, fd, prev_pipe, envp);
            prev_pipe = fd[0];
        }
        else
            cmd->pid = exec_fork(cmd->args, NULL, prev_pipe, envp);
        if (cmd->pid == -1)
            break ;    //handle error
        if (cmd->next)
            close(fd[1]);
        else if (prev_pipe != -1)
            close(prev_pipe);
        cmd = cmd->next;
    }
    cmd = cmds;
    while(cmd)
    {
        if (cmd->pid == -1)
            return (-1);
        waitpid(cmd->pid, status, 0);
        cmd = cmd->next;
    }
    return (WEXITSTATUS(*status));
}
