#include "minishell.h"

volatile sig_atomic_t g_signal = 0;
static struct termios original_term;

/*
        Context	SIGINT          SIGQUIT
Parent  shell	custom handler	ignored
Normal  child	default         default
Heredoc child	default         ignored
*/


/*
Handle ctrl-C, ctrl-D and ctrl-\ which should behave like in bash.
• In interactive mode:
◦ ctrl-C (SIGINT) displays a new prompt on a new line.
exit status 128 + SIGINT

Parent shell:
Do not exit
Reset prompt
Child process:
Default behavior → terminate

◦ ctrl-D exits the shell.
This is not a signal. End Of File (EOF) on stdin

In bash:
At empty prompt → exit shell
While typing → behaves like EOF
In heredoc → ends input

If readline() returns NULL → treat as Ctrl+D
At prompt:
Print exit
Exit shell

◦ ctrl-\ (SIGQUIT) does nothing.

Default behavior: terminate + core dump

In bash:
At prompt → ignored
While command runs → child may terminate
Bash suppresses the “Quit (core dumped)” message in many cases

Parent shell:
Ignore it
Child:
Default handling (or let kernel do it)

-----
Parent shell
Ignores SIGINT, SIGQUIT
Handles Ctrl+D via EOF
Never dies from terminal signals

Child process
Restores default signal handling
Receives SIGINT / SIGQUIT
Terminates normally

*/



/*
DESCRIPTION:
    It is a wrapper function that calls sigaction to set up a signal handler.
    If the sigaction fails, it prints an error message and exits the program.
    
    Minishell exits itself on sigaction initialisation error.
    sigaction() failing means your shell cannot control signals, and a shell
    without proper signal handling is broken and unsafe.

    sigaction blocks SIGINT while processing SIGQUIT and vice-versa.
    signature used for the sigaction handler:
        union __sigaction_u = void (*__sa_handler)(int);

    Without SA_RESTART (flags = 0):
    System calls (like read(), write(), readline()) are interrupted by signals
    They return with EINTR error
    This is what you want for SIGINT — so readline() returns immediately when Ctrl+C is pressed
    With SA_RESTART:
    System calls automatically restart after the signal handler returns
    This is NOT what you want for SIGINT — readline() would continue waiting instead of returning
*/    

void    setup_handler(int sig, void (*handler)(int))
{
    struct sigaction sa;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    if (sig == SIGINT)
        sigaddset(&sa.sa_mask, SIGQUIT);
    else if (sig == SIGQUIT)
        sigaddset(&sa.sa_mask, SIGINT);
    if (sig == SIGINT)
        sa.sa_flags = 0;
    else
        sa.sa_flags = SA_RESTART;
    if (sigaction(sig, &sa, NULL) < 0)
    {
        perror(MINI);
        exit(EXIT_FAILURE);
    }
}

/*
DESCRIPTION:
    It is the signal handler for the client that receives the acknowledgment signal from the server.

    In interactive mode:
    SIGINT(ctrl-C), displays a new prompt on a new line.
    SIGQUIT(ctrl-\), does nothing.
    ctrl-D exits the shell. This is not a signal. End Of File (EOF) on stdin

    Ctrl+D:
        Ends heredoc early
    Ctrl+C:
        Cancels heredoc
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_IGN);

    set SA_RESTART where appropriate
*/

void    parent_handler(int sig)
{
    if (sig == SIGINT)
    {
        g_signal = sig;
        write(1, "\n", 1);
        rl_replace_line("", 0);
        rl_on_new_line();
        rl_redisplay();
    }
}

/*
using sigaction to setup did not work, so using signal instead.
term.c_lflag &= ~ECHOCTL; // prevents terminal from printing ^C before handler runs.
*/

void    init_signals(void)
{
    struct termios term;

    tcgetattr(STDIN_FILENO, &original_term);
    term = original_term;
    term.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSANOW, &term);
    setup_handler(SIGINT, parent_handler);
    setup_handler(SIGQUIT, SIG_IGN);
}

/*

*/
void    handle_parent_signal(int *status)
{
    *status = 1;
    g_signal = 0;
}
