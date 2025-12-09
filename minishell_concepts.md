# MINISHELL
[Bash Reference Manual](https://www.gnu.org/software/bash/manual/bash.html)
https://cgit.git.savannah.gnu.org/cgit/bash.git/tree/

parser.y

## Duplicate fd operator

`>&` = duplicate file descriptor operator

Examples:
```sh
2>&1    # FD 2 duplicates FD 1
1>&2    # FD 1 duplicates FD 2
3>&1    # FD 3 duplicates FD 1

2>&1    # redirect 2 to FD 1
2>1     # redirect 2 to a FILE named "1"
2>&-    # close FD 2
```
Syntax      Meaning
`2>file`    redirect stderr to file named “file”
`2>1`       redirect stderr to file named “1”
`2>&1`      redirect stderr to the current stdout FD
`2>&-`	    close stderr
`>file`	    redirect stdout to file
`>&1`       duplicate FD 1 onto FD 1 (no-op)

## REPL
Read–eval–print loop, also termed an interactive toplevel or language shell, is a simple interactive computer programming environment that takes single user inputs, executes them, and returns the result to the user

# PART 1 - PROCESSES: fork()
fork() duplicates the current process.
```c
pid_t pid = fork();

if (pid == 0)
{
    // CHILD
}
else
{
    // PARENT
}
```
After fork():
the parent continues
the child starts running the same code
both start right after the fork call
variables are copied
file descriptors are shared but independent

**shared but independant meaning:**
They point to the SAME underlying file/pipe
BUT
- The parent and child have their OWN file descriptor numbers and positions.
Better explanation:
- Shared:
both point to the SAME kernel file object
both see the same data (e.g. same pipe or same file)
closing one does not close the other
- Independent:
if the child closes fd 3, the parent’s fd 3 still exists
if the child redirects fd 1 with dup2, it doesn’t affect the parent
each can close or modify descriptors without affecting the other

# PART 2 - EXEC: execve()
After fork(), usually the child runs a command using:
execve(path, argv, envp);
If execve succeeds:
the current process image is replaced by the program
no return occurs
If it fails:
it returns -1
```c
// Child logic usually looks like this:
if (fork() == 0)
{
    execve(cmd_path, cmd_args, envp);
    perror("execve"); 
    exit(1);
}

// Parent waits:
waitpid(pid, NULL, 0);
```

# PART 3 - PIPE()
A pipe creates two file descriptors:
```c
int fd[2];
pipe(fd);
```
fd[0] → read end
fd[1] → write end
Whatever is written to fd[1] can be read from fd[0].
Example:
```sh
echo hello | wc -l
```
pipe:
[write] ---> [read]

# PART 4 - dup2(): redirect stdin/stdout
The most important system call for minishell:
```c
dup2(old_fd, new_fd);
```
This makes new_fd refer to whatever old_fd used to.
Common usage:
Redirect stdout to a pipe:
```c
dup2(pipefd[1], 1);
```
→ Now anything printed with printf or write(1, …) goes inside the pipe.
Redirect stdin:
```c
dup2(pipefd[0], 0);
```
→ Now reading from stdin actually reads from the pipe.
After calling dup2, always close() the original descriptor.

# PART 5 - Putting it all together: a simple pipeline
```c
ls | wc -l
```
Pipeline steps:
```c
// Step 1: create a pipe
pipe(fd); // fd[0] = read, fd[1] = write
// Step 2: fork FIRST CHILD (left command: ls)
pid1 = fork();
if (pid1 == 0)
{
    dup2(fd[1], 1);  // redirect stdout to pipe
    close(fd[0]);    // close unused read
    close(fd[1]);
    execve("/bin/ls", ls_argv, envp);
}
// Step 3: fork SECOND CHILD (right command: wc -l)
pid2 = fork();
if (pid2 == 0)
{
    dup2(fd[0], 0);  // redirect stdin to pipe
    close(fd[1]);    // close unused write
    close(fd[0]);
    execve("/usr/bin/wc", wc_argv, envp);
}
// Step 4: parent closes everything
close(fd[0]);
close(fd[1]);
waitpid(pid1, NULL, 0);
waitpid(pid2, NULL, 0);
```

# PART 6 - MULTIPLE PIPES (cmd1 | cmd2 | cmd3 | ...)
General rule:
For N commands, you need N - 1 pipes.
Example:
a | b | c | d
pipes:
p1  p2  p3

Each command gets:
A pipe to read from (except the first)
A pipe to write to (except the last)

This gives:
Command	stdin from	stdout to
a	normal stdin	p1 write
b	p1 read	p2 write
c	p2 read	p3 write
d	p3 read	normal stdout

The logic is:
- Create all pipes
- Fork each command
- Connect the correct pipes to stdin/stdout with dup2
- Close unused pipes
- execve

# PART 7 - Redirections
Redirect output to file:
```sh
ls > file.txt
```
Implementation:
```c
int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
dup2(fd, 1); // redirect stdout to file
close(fd);
execve(...);
```
Append:
```sh
echo hi >> file.txt
```
```c
open("file.txt", O_WRONLY | O_CREAT | O_APPEND);
```
Redirect input:
```sh
cat < infile
```
```c
fd = open("infile", O_RDONLY);
dup2(fd, 0);
close(fd);
execve(...);
```

# PART 8 - HEREDOC (<<)
```dh
cat << EOF
hello
world
EOF
```

Steps:
- Create a pipe
- Write the heredoc lines into the write end
- Close it
- Child redirects stdin from the read end

Minishell must do:
Read input line-by-line until limiter
Send lines into a pipe

# PART 9 - BUILTINS
Some builtins must run without forking:
* echo with option -n
* cd with only relative or absolute path
* pwd
* export
* unset
* env with no options ot arguments
* exit
Because they modify the shell’s environment/state.
But builtins inside pipelines must fork:
```sh
export x=5 | cat
```
Because each side of a pipe must be its own process.

# PART 10 - Signal Handling in Minishell
Key behavior:
- Ctrl-C interrupts prompt but NOT the shell
- *Ctrl-* does nothing in prompt
- In child processes, these signals revert to default behavior

Typical setup:
```
Parent shell:
SIGINT -> prompt reset
SIGQUIT -> ignored

Child commands:
SIGINT -> kill child
SIGQUIT -> core dump
```
