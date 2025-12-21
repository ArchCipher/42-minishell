#include "minishell.h"

/*
heredoc reads from terminal and writes to write end of a pipe and later read by command via stdin.
not write soon but save to buffer.
*/

int exec_heredoc(char *limiter, int fd)
{
    char *line;
    // char *buffer;

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

int handle_heredoc(t_cmd *cmds)
{
    int     fd[2];
    pid_t   pid;
    t_redir *current;
    int     status;

    while(cmds)
    {
        current = cmds->redirs;
        while(current)
        {
            if (current->flag == heredoc)
            {
                if (pipe(fd) == -1)
                    return (perror(MINI), 0);
                pid = fork();
                if (pid == -1)
                {
                    close(fd[0]);
                    close(fd[1]);
                    perror(MINI);
                    return (0);
                }
                if (pid == 0)
                {
                    setup_handler(SIGINT, SIG_DFL);
                    setup_handler(SIGQUIT, SIG_IGN);
                    close(fd[0]);
                    exit (exec_heredoc(current->file, fd[1]));
                }
                setup_handler(SIGINT, SIG_IGN);
                close(fd[1]);
                waitpid(pid, &status, 0);
                setup_handler(SIGINT, parent_handler);
                if (WIFSIGNALED(status) || WEXITSTATUS(status))
                    return (close(fd[0]), write(1, "\n", 1), 0);
                current->fd = fd[0];
            }
            current = current->next;
        }
        cmds = cmds->next;
    }
    return (1);
}

/*
chatgpt: 

What actually happens:
Prompt for A
User types input (or hits Ctrl-C)
Only if A succeeds, prompt for B
If any heredoc is interrupted, the entire command is aborted
If you delay waitpid():
Multiple heredoc children could run
Prompts interleave
SIGINT handling breaks
You lose which heredoc failed
$? becomes wrong
Correct behavior (bash-compatible)
For each heredoc redirection:
pipe()
fork()
Child:
default SIGINT
read lines, write to pipe
exit
Parent:
close write end
waitpid(child)
check:
WIFSIGNALED(SIGINT) → abort whole command
non-zero exit → abort
store read-end FD in redir
Only then move to the next heredoc.
Why not wait at the end?
Waiting later causes:
Problem	Why it breaks
Zombie processes	children exit early
Ctrl-C ignored	parent doesn’t see interruption
Wrong $?	status lost
Wrong prompts	heredocs overlap
Mental rule (remember this)
A heredoc is synchronous.
It blocks parsing until it finishes.
Practical takeaway
✔️ One heredoc → fork → wait immediately
✔️ On SIGINT → stop building commands
✔️ Execution must not happen
If you want, next I can:
sketch the correct heredoc control flow (no code)
explain how bash cancels pipelines on heredoc SIGINT
help you store heredoc status cleanly with your g_signal rule
*/