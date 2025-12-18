#include "minishell.h"

/*
handle redirections < > << >>
implement redirection operator logic
execve wrapper
*/

int exec_built_in(t_cmd *cmd)
{
    if (cmd->built_in == ECHO)
        exec_echo(cmd->args + 1);
    else if (cmd->built_in == CD)
        exec_cd(cmd->args[1]);
    else if (cmd->built_in == PWD)
        exec_pwd();
    // no skeleton yet
    // else if (cmd->built_in == EXPORT)
    //     exec_export();
    // else if (cmd->built_in == UNSET)
    //     exec_unset();
    // else if (cmd->built_in == ENV)
    //     exec_env();
    else if (cmd->built_in == EXIT)
        exec_exit(cmd->args[1]);
    return (0); // return value on error as well
}

/*
returns -1 if not a builtin
*/

int is_builtin(char *s)
{
    if (ft_strcmp(s, "echo") == 0)
        return (ECHO);
    else if (ft_strcmp(s, "cd") == 0)
        return (CD);
    else if (ft_strcmp(s, "pwd") == 0)
        return (PWD);
    else if (ft_strcmp(s, "export") == 0)
        return (EXPORT);
    else if (ft_strcmp(s, "unset") == 0)
        return (UNSET);
    else if (ft_strcmp(s, "env") == 0)
        return (ENV);
    else if(ft_strcmp(s, "exit") == 0)
        return (EXIT);
    return (-1);
}

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

    This is child only logic and doesn't return invalid path. It exits on error.
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
if no infile:
no such file or directory: infile_name
*/
int setup_redirs(t_redir *redirs)
{
    int fd;

    if (!redirs)
        return (0);
    while(redirs)
    {
        if (redirs->flag == redir_in)
        fd = open(redirs->file, O_RDONLY);
        else if (redirs->flag == redir_out || redirs->flag == heredoc)
            fd = open(redirs->file, O_WRONLY | O_CREAT | O_TRUNC, 0660);
        else
            fd = open(redirs->file, O_WRONLY | O_CREAT | O_APPEND, 0660);
        if (fd == -1)
            return (-1);
        if (((redirs->flag == redir_in || redirs->flag == heredoc) && dup2(fd, STDIN_FILENO) == -1) || ((redirs->flag == redir_out || redirs->flag == append) && dup2(fd, STDOUT_FILENO) == -1))
        {
            close(fd);
            return (-1);
        }
        close(fd);
        redirs = redirs->next;
    }
    return (0);
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

int exec_fork(t_cmd *cmd, int *fd, int prev_fd, char **envp)
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
        if (setup_redirs(cmd->redirs) == -1)
            exit(1);
        path = cmd->args[0];
        if (cmd->built_in != -1)
            exit(exec_built_in(cmd));
        if (ft_strchr(cmd->args[0], '/'))
            check_path_validity(path, false);
        else
            path = build_path(cmd->args[0]);
        execve(path, cmd->args, envp); // should never return
        // if returned
        if (!ft_strchr(cmd->args[0], '/'))
            free(path);
        perror("minishell: ");
        exit(1);
    }
    return (pid);
}

/*
WAITPID:
    to prevent zombie processes, the below code was not used,
    as this can return early if one of the child processes fail.
    if (waitpid(cmd->pid, &status, 0) == -1)
        return (-1); // handle error
*/

int return_status(t_cmd *cmds, int *status)
{
    while(cmds)
    {
        if (cmds->pid == -1)
            return (-1);
        waitpid(cmds->pid, status, 0);
        cmds = cmds->next;
    }
    return (WEXITSTATUS(*status));
}

int exec_in_parent(t_cmd *cmd)
{
    int actual_stdout;
    int ret;

    if ((actual_stdout = dup(STDOUT_FILENO)) == -1)
        return (-1);
    if (setup_redirs(cmd->redirs) == -1)
        return (1); // is returning 1 okay?
    ret = exec_built_in(cmd);
    if (dup2(actual_stdout, STDOUT_FILENO) == -1)
        return (-1);
    return (ret);
}

/*
PIPE: 
// fd[2] first fd i.e., fd[0] connects to read end and second i.e., fd[1] to write end.
// pipe returns 0 on succes, -1 on error & errno is set.
*/

// separate execve wrapper for built-in and executables
int exec_cmds(t_cmd *cmds, char **envp, int *status)
{
    int     fd[2];
    int     prev_pipe;
    t_cmd   *cmd;
    bool    need_pipe;

    cmd = cmds;
    prev_pipe = -1;
    need_pipe = 0;
    if (cmd->next)
        need_pipe = 1;
    while(cmd)
    {
        cmd->built_in = is_builtin(cmd->args[0]);
        if (cmd->built_in != -1 && !need_pipe)
        {
            return (exec_in_parent(cmd));
        }
        else
        {
            if (cmd->next)
            {
                if (pipe(fd) == -1)
                    return (-1); // handle error
                cmd->pid = exec_fork(cmd, fd, prev_pipe, envp);
                prev_pipe = fd[0];
            }
            else
                cmd->pid = exec_fork(cmd, NULL, prev_pipe, envp);
            if (cmd->pid == -1)
                break ;    //handle error
            if (cmd->next)
                close(fd[1]);
            else if (prev_pipe != -1)
                close(prev_pipe); // read end
        }
        cmd = cmd->next;
    }
    return (return_status(cmds, status));
}
