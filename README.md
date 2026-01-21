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
- **Control Flow:** Logical operators (`&&`, `||`), command chaining, subshell execution
- **Environment Variables:** Variable expansion, `$?`, `$$`, `$USER`, etc.
- **Built-in Commands:** `cd`, `echo`, `pwd`, `export`, `unset`, `env`, `exit`
- **Error Handling:** Proper error messages, exit codes, error recovery

---

## Features Implemented

### Command Execution
- Execute external programs via `execve()`
- PATH resolution for command lookup
- Proper argument passing
- Exit code handling

### Pipes
- Multiple pipes in sequence (`cmd1 | cmd2 | cmd3`)
- Proper file descriptor management
- Process synchronization
- Signal handling in pipelines

### Redirections
- Output redirection (`>`)
- Append redirection (`>>`)
- Input redirection (`<`)
- Heredoc (`<<`)
- Multiple redirections
- Redirection with pipes

### Environment Variables
- Variable expansion (`$VAR`, `${VAR}`)
- Special variables (`$?`, `$$`, `$USER`)
- Variable assignment (`export VAR=value`)
- Variable unsetting (`unset VAR`)

### Built-in Commands
- `cd` - Change directory (with `-` support)
- `echo` - Print with `-n` flag
- `pwd` - Print working directory
- `export` - Set environment variables
- `unset` - Remove environment variables
- `env` - Print environment
- `exit` - Exit shell with code

### Advanced Features
- Logical operators (`&&`, `||`)
- Parentheses for command grouping
- Quoting (`'`, `"`)
- Signal handling (Ctrl+C, Ctrl+D, Ctrl+\)
- Proper error messages
- Exit code propagation

---

## Project Structure

```
minishell/
├── src/
│   ├── exec.c              # Command execution and process management
│   ├── exec_child.c        # Child process setup and execution
│   ├── parser.c            # Command parsing and AST building
│   ├── lexer.c             # Tokenization
│   ├── expand.c            # Environment variable expansion
│   ├── builtins/           # Built-in command implementations
│   └── ...                 # Additional source files
├── libft/                  # Custom library functions
├── dprintf/                # Printf implementation for error output
├── minishell.h             # Main header file
├── types.h                 # Type definitions
└── Makefile                # Build configuration
```

**Key Components:**
- **Parser:** Builds abstract syntax tree from command line input
- **Executor:** Manages process creation, pipes, and redirections
- **Built-ins:** Implements shell built-in commands
- **Expander:** Handles environment variable expansion

---

## Architecture

### Core Components

**1. Lexer ([`lexer.c`](src/lexer.c))**
- Tokenizes input string
- Identifies commands, operators, redirections
- Handles quotes and special characters

**2. Parser ([`parser.c`](src/parser.c))**
- Builds abstract syntax tree
- Validates syntax
- Organizes commands and operators

**3. Executor ([`exec.c`](src/exec.c), [`exec_child.c`](src/exec_child.c))**
- Executes command trees
- Manages process creation
- Handles pipes and redirections
- Waits for process completion

**4. Built-ins (`builtins/`)**
- Implements shell built-in commands
- Handles environment variable manipulation
- Manages shell state

**5. Expander ([`parser_expand.c`](src/parser_expand.c))**
- Expands environment variables
- Handles special variables
- Processes quotes

**6. Signal Handler ([`signal.c`](src/signal.c))**
- Handles SIGINT (Ctrl+C)
- Handles SIGQUIT (Ctrl+\)
- Manages signal blocking during execution

---

## Technical Implementation

### Lexing and Tokenization

See [`lexer.c`](src/lexer.c) for input tokenization and operator identification.

### Process Management

See [`exec.c`](src/exec.c) for fork and execute implementation.

### Pipe Implementation

See [`exec_child.c`](src/exec_child.c) for pipe setup and file descriptor management.

### Signal Handling

See [`signal.c`](src/signal.c) for signal handler setup and blocking.

---

## Key Challenges & Solutions

### 1. Pipe Chain Execution
**Managing multiple processes in a pipe chain**

I tracked pipe head, waited only after all commands in chain are forked, and checked next connector `next->con` to determine end of pipe chain

### 2. Environment Variable Expansion
**Expanding variables within quoted strings correctly**

I initially expanded all variables in the parser stage before execution. When I tried to implement logical operators (`&&` and `||`), this caused bugs because expansion needed to happen after command execution to properly handle exit codes and variable values that depend on previous commands. I moved the expansion logic to the execution stage. I tracked the quote state during parsing and only expanded variables when appropriate based on the current quote context

### 3. Signal Handling
**Ctrl+C should interrupt the current command, not exit the shell**

I looked for signal-safe options but couldn't find an alternative. I later learned that readline is documented in glibc as signal-safe and was used only to interrupt readline, so I used readline functions to interrupt readline. I blocked signals in the parent process, allowed them in child processes, and restored handlers after execution completes

### 4. Zombie Process Prevention
**Signal handling causing zombie processes**

I had not added signal handling in command execution (although I did implement it in heredoc), which caused the shell to receive Ctrl+C signals when they were supposed to be received by the child process, like for `cat`. This caused zombie processes to exist. I used `watch -n 0.2 pstree -p <pid>` to check this and later resolved it.

### 5. File Descriptor Management
**Properly closing file descriptors to prevent leaks**

I tracked all opened file descriptors and made sure to close them in both parent and child processes appropriately

### 6. Error Handling
**Proper error messages matching bash behavior**

I studied bash error messages and implemented matching error handling to ensure compatibility

---

## Compilation & Usage

```bash
# Build minishell
make

# Run minishell
./minishell
```

---

## Testing

The project includes comprehensive testing:
- **scan-build** - Static analysis tool used during development to catch memory leaks, uninitialized variables, and other potential bugs
- **AddressSanitizer (ASan)** - Runtime memory error detector used to catch buffer overflows, use-after-free, and other memory safety issues
- **[minishell_tester](https://github.com/LucasKuhn/minishell_tester)** - Community tester with optimal test cases
- **[42_minishell_tester](https://github.com/zstenger93/42_minishell_tester)** - Community tester with extensive test cases
- Custom test cases for edge cases and error handling

**Test Coverage:**
- Command execution
- Pipes and redirections
- Built-in commands
- Environment variables
- Signal handling
- Error handling
- Edge cases

---

## Technical Highlights

- **Robust Parsing:** Handles complex command lines with proper syntax validation
- **Efficient Execution:** Proper process management and resource cleanup
- **Signal Safety:** Correct signal handling preventing hangs and crashes
- **Memory Management:** Careful allocation and deallocation, leak-free
- **Error Recovery:** Graceful error handling with appropriate exit codes
- **Bash Compatibility:** Matches bash behavior for common operations

---

*This project demonstrates mastery of system programming, process management, and complex software architecture in C.*
