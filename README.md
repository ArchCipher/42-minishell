# <p align="center"> Minishell - Custom Shell Implementation </p>

**42 Project | System Programming & Process Management**  
**Objective:** Create a simplified version of bash, implementing command execution, pipes, redirections, environment variables, and signal handling.

---

## Overview

**Minishell** is one of the most complex 42 projects, requiring deep understanding of system programming, process management, and shell internals. This project implements a functional shell that can execute commands, handle pipes, redirections, environment variables, and built-in commands.

---

## Skills Demonstrated

- **Process Management:** `fork()`, `execve()`, `waitpid()`, process groups
- **Inter-Process Communication:** Pipes, file descriptor manipulation
- **Signal Handling:** `SIGINT` (Ctrl+C), `SIGQUIT` (Ctrl+\), signal blocking
- **File I/O:** Redirections (`>`, `>>`, `<`), heredoc (`<<`), file descriptor management
- **Parsing:** Complex command line parsing, tokenization, syntax validation
- **Environment Variables:** Variable expansion, `$?`, `$$`, `$USER`, etc.
- **Built-in Commands:** `cd`, `echo`, `pwd`, `export`, `unset`, `env`, `exit`
- **Error Handling:** Proper error messages, exit codes, error recovery

---

## Features Implemented

### Command Execution
- ✅ Execute external programs via `execve()`
- ✅ PATH resolution for command lookup
- ✅ Proper argument passing
- ✅ Exit code handling

### Pipes
- ✅ Multiple pipes in sequence (`cmd1 | cmd2 | cmd3`)
- ✅ Proper file descriptor management
- ✅ Process synchronization
- ✅ Signal handling in pipelines

### Redirections
- ✅ Output redirection (`>`)
- ✅ Append redirection (`>>`)
- ✅ Input redirection (`<`)
- ✅ Heredoc (`<<`)
- ✅ Multiple redirections
- ✅ Redirection with pipes

### Environment Variables
- ✅ Variable expansion (`$VAR`, `${VAR}`)
- ✅ Special variables (`$?`, `$$`, `$USER`)
- ✅ Variable assignment (`export VAR=value`)
- ✅ Variable unsetting (`unset VAR`)

### Built-in Commands
- ✅ `cd` - Change directory (with `-` support)
- ✅ `echo` - Print with `-n` flag
- ✅ `pwd` - Print working directory
- ✅ `export` - Set environment variables
- ✅ `unset` - Remove environment variables
- ✅ `env` - Print environment
- ✅ `exit` - Exit shell with code

### Advanced Features
- ✅ Logical operators (`&&`, `||`)
- ✅ Parentheses for command grouping
- ✅ Quoting (`'`, `"`)
- ✅ Signal handling (Ctrl+C, Ctrl+D, Ctrl+\)
- ✅ Proper error messages
- ✅ Exit code propagation

---

## Project Structure

```
minishell/
├── src/
│   ├── exec.c              # Command execution and process management
│   ├── exec_child.c         # Child process setup and execution
│   ├── parser.c             # Command parsing and AST building
│   ├── lexer.c              # Tokenization
│   ├── expand.c             # Environment variable expansion
│   ├── builtins/            # Built-in command implementations
│   └── ...                  # Additional source files
├── libft/                   # Custom library functions
├── dprintf/                 # Printf implementation for error output
├── minishell.h              # Main header file
├── types.h                   # Type definitions
└── Makefile                 # Build configuration
```

**Key Components:**
- **Parser:** Builds abstract syntax tree from command line input
- **Executor:** Manages process creation, pipes, and redirections
- **Built-ins:** Implements shell built-in commands
- **Expander:** Handles environment variable expansion

---

## Architecture

### Core Components

**1. Lexer (`lexer.c`)**
- Tokenizes input string
- Identifies commands, operators, redirections
- Handles quotes and special characters

**2. Parser (`parser.c`)**
- Builds abstract syntax tree
- Validates syntax
- Organizes commands and operators

**3. Executor (`exec.c`, `exec_child.c`)**
- Executes command trees
- Manages process creation
- Handles pipes and redirections
- Waits for process completion

**4. Built-ins (`builtins/`)**
- Implements shell built-in commands
- Handles environment variable manipulation
- Manages shell state

**5. Expander (`expand.c`)**
- Expands environment variables
- Handles special variables
- Processes quotes

**6. Signal Handler (`signals.c`)**
- Handles SIGINT (Ctrl+C)
- Handles SIGQUIT (Ctrl+\)
- Manages signal blocking during execution

---

## Technical Implementation

### Process Management

```c
// Fork and execute command
pid_t pid = fork();
if (pid == 0)
{
    // Child: setup redirections and execute
    dup2(fd_in, STDIN_FILENO);
    dup2(fd_out, STDOUT_FILENO);
    execve(cmd_path, args, env);
}
else
{
    // Parent: wait for child
    waitpid(pid, &status, 0);
}
```

### Pipe Implementation

```c
// Create pipe
int pipe_fd[2];
pipe(pipe_fd);

// Child 1: write to pipe
dup2(pipe_fd[1], STDOUT_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);

// Child 2: read from pipe
dup2(pipe_fd[0], STDIN_FILENO);
close(pipe_fd[0]);
close(pipe_fd[1]);
```

### Signal Handling

```c
// Setup signal handlers
signal(SIGINT, handle_sigint);
signal(SIGQUIT, handle_sigquit);

// Block signals during execution
sigemptyset(&sa.sa_mask);
sigaddset(&sa.sa_mask, SIGINT);
sigaddset(&sa.sa_mask, SIGQUIT);
```

---

## Key Challenges & Solutions

### Challenge 1: Pipe Chain Execution
**Problem:** Managing multiple processes in a pipe chain  
**Solution:** Track pipe head, wait only after all commands in chain are forked, check `next->con` to determine end of pipe chain

### Challenge 2: Signal Handling
**Problem:** Ctrl+C should interrupt current command, not exit shell  
**Solution:** Block signals in parent, allow in child, restore handlers after execution

### Challenge 3: File Descriptor Management
**Problem:** Properly closing file descriptors to prevent leaks  
**Solution:** Track all opened FDs, close in both parent and child appropriately

### Challenge 4: Environment Variable Expansion
**Problem:** Expanding variables within quoted strings correctly  
**Solution:** Track quote state during parsing, expand variables only when appropriate

### Challenge 5: Error Handling
**Problem:** Proper error messages matching bash behavior  
**Solution:** Study bash error messages, implement matching error handling

### Challenge 6: EINTR Handling
**Problem:** `waitpid()` interrupted by signals causing hangs  
**Solution:** Retry `waitpid()` when `errno == EINTR`

---

## Compilation & Usage

```bash
# Build minishell
make

# Run minishell
./minishell
```

**Example Usage:**
```bash
$> ls -la | grep "\.c" | wc -l
$> echo $USER
$> export TEST=hello && echo $TEST
$> cd /tmp && pwd
$> cat file.txt > output.txt
$> cat << EOF
> hello
> world
> EOF
```

---

## Testing

The project includes comprehensive testing:
- **[minishell_tester](https://github.com/LucasKuhn/minishell_tester)** - Community tester with optimal test cases
- **[42_minishell_tester](https://github.com/zstenger93/42_minishell_tester)** - Community tester with extensive test cases
- Custom test cases for edge cases and error handling

**Test Coverage:**
- ✅ Command execution
- ✅ Pipes and redirections
- ✅ Built-in commands
- ✅ Environment variables
- ✅ Signal handling
- ✅ Error handling
- ✅ Edge cases

---

## Technical Highlights

- **Robust Parsing:** Handles complex command lines with proper syntax validation
- **Efficient Execution:** Proper process management and resource cleanup
- **Signal Safety:** Correct signal handling preventing hangs and crashes
- **Memory Management:** Careful allocation and deallocation, leak-free
- **Error Recovery:** Graceful error handling with appropriate exit codes
- **Bash Compatibility:** Matches bash behavior for common operations

---

## Project Status

✅ **Completed** - Full implementation with all mandatory and bonus features

---

*This project demonstrates mastery of system programming, process management, and complex software architecture in C.*
