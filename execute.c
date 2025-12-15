#include "minishell.h"

/*
path lookup
pipe handling for commands
handle pipes |
implement forking logic
handle redirections < > << >>
implement redirection operator logic
execve wrapper
*/

/*
Execution Process: After building the AST, you can traverse the list of commands and:
Create pipes: For each pair of consecutive commands (e.g., cmd1 | cmd2), you create a pipe using the pipe() system call.
Fork processes: You fork() for each command in the pipeline.
Redirect stdout: Redirect the output of each command to the pipe using dup2() and connect the input of each command to the pipe.
Execute: Use execve() to execute the commands.
*/

// fd[2] first fd i.e., fd[0] connects to read end and second i.e., fd[1] to write end.
// pipe returns 0 on succes, -1 on error & errno is set.

/*
int execve(const char *path, char *const argv[], char *const envp[]);
When a program is executed as a result of an execve() call, it is entered as follows:
    int main(int argc, char **argv, char **envp);

where argc is the number of elements in argv (the ``arg count'') and 
argv points to the array of character pointers to the arguments themselves.

As the execve() function overlays the current process image  with a new process image, the successful call has no process to return to.  If execve()
oes return to the calling process, an error has occurred; the return value will be -1 and the global variable errno is set to indicate the error.
*/

/*
For each t_cmd:
If cmd->args[0] contains /
    Try execve(cmd->args[0], args, envp)
Else
    build_path

Check access(path, X_OK)
int access(const char *path, int mode); // returns -1 on error and 0 on success

for alsolute or relative path search
F_OK for file existence test: command not found
X_OK for execute/search permission: Permission denied
Found but is directory:	Is a directory

stat(path) fails → No such file or directory
is directory → Is a directory
not executable → Permission denied
else → execve

If found → execve

when path is empty "", it means current directory-> create full string with ""./" such as " "./cmd"
*/

int exec_fork(t_exec exec, int *fd, char **envp)
{
    pid_t pid;

    pid = fork();
    if (pid == -1)
        return (-1); // handle error
    if (pid == 0)
    {
        if (exec.write_fd != -1 && (dup2(fd[1], exec.write_fd) == - 1))
            exit(-1); // handle_error
        if (exec.read_fd != -1 && (dup2(fd[0], exec.read_fd) == -1))
            exit(-1); // handle error
        close(fd[0]);
        close(fd[1]);
        execve(exec.path, exec.av, envp); // should never return
        exit(-1);   // if returned
    }
    return (pid);
}

t_exec  init_exec(char *path, char **av, int write_fd, int read_fd)
{
    t_exec  exec;

    exec.path = path;
    exec.av = av;
    exec.write_fd = write_fd;
    exec.read_fd = read_fd;
    return (exec);
}

int exec_pipe(char **envp)
{
    t_exec  exec;
    int     fd[2];
    pid_t   pid1;
    pid_t   pid2;
    int     status;
    char    *path1 = "/bin/ls";
    char    *path2 = "/usr/bin/wc";
    char    *av1[3] = {"ls", "-ls", NULL};
    char    *av2[3] = {"wc", "-l", NULL};

    if (pipe(fd) == -1)
        return (-1); // handle error
    exec = init_exec(path1, av1, STDOUT_FILENO, -1);
    pid1 = exec_fork(exec, fd, envp);
    if (pid1 == -1)
        return (-1);    //handle error (need to close pipe fds?)
    exec = init_exec(path2, av2, -1, STDIN_FILENO);
    pid2 = exec_fork(exec, fd, envp);
    if (pid2 == -1)
        return (-1);    // handle error (need to close pipe fds?)
    close(fd[0]);
    close(fd[1]);
    if (waitpid(pid1, &status, 0) == -1)
        return (-1); // handle error
    if (waitpid(pid2, &status, 0) == -1)
        return (-1); // handle error
    return (WEXITSTATUS(status));
}
/*
Read $PATH
Split by :
For each directory:
Build dir + "/" + cmd
*/
char    *build_path(char *filename)
{
    char    *path;
    char    *full_path;
    char    *p;
    char    *new_path;
    struct stat buf;

    // maybe use strndup instead as this is only place i use strdup()
    if (!(path = getenv("PATH")) || !(full_path = ft_strdup(path))) 
        return (NULL);
    path = ft_strtok_r(full_path, ":", &p);
    while(path)
    {
        new_path = ft_strjoin3(path, "/", filename);
        if (!new_path)
            return (free(full_path), NULL);
        if(stat(new_path, &buf) == 0)
            break;
        free(new_path);
        new_path = NULL;
        path = ft_strtok_r(NULL, ":", &p);
    }
    free(full_path);
    return (new_path);
}

// void exec_cmds(t_cmd *cmds)
// {
//     t_cmd   *current;
//     char    *path;

//     if (!cmds)
//         return ;
//     current = cmds;
//     if (current)
//     {
//         path = build_path(current->args[0]);
//         printf("path: %s\n", path);
//         free(path);
//     }
//     // while(current)
//     // {
//         // if (current->next)
//         //     create_pipes(current, current->next);
//     // }
// }

int main(int ac, char **av, char **envp)
{
    char    *path;

    int ret = exec_pipe(envp);
    printf("return: %d\n", ret);
    path = build_path("ls");
    printf("path: %s\n", path);
    free(path);
}