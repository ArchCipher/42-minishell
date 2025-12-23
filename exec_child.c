#include "minishell.h"

static char    *get_valid_path(char *filename);
static char    *build_path(char *filename);
static void    child_dup_error(int *fd, int prev_fd);
static void    child_path_error(char *filename, char *msg, int exit_code, char *path);

// separate execve wrapper for built-in and executables

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

    if (setup_handler(SIGINT, SIG_DFL) == -1 || setup_handler(SIGQUIT, SIG_DFL) == -1)
        exit(1);
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

static char    *get_valid_path(char *filename)
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

/*
DESCRIPTION
    Retrieves $PATH using getenv, splits by : using ft_strtok_r.
    For each directory in $PATH, builds dir + "/" + cmd.

    When path is empty "", it means current directory-> create full string with ""./" such as " "./cmd"

    This is child only logic and doesn't return invalid path. It exits on error.
    Caution: !!!cannot be used in parent process!!!
*/
static char    *build_path(char *filename)
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
DESCRIPTION:
    closes fds, prints error message and exits child on error.
*/

static void    child_dup_error(int *fd, int prev_fd)
{
    close_pipe_fds(fd, prev_fd);
    perror(MINI);
    exit(1);
}

static void    child_path_error(char *filename, char *msg, int exit_code, char *path)
{
    printf("%s: %s: %s\n", MINI, filename, msg);
    if (!ft_strchr(filename, '/') && path)
        free(path);
    exit(exit_code); 
}
