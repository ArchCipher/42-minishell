/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:20 by kmurugan          #+#    #+#             */
/*   Updated: 2025/12/28 19:58:26 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

/*
NOTES:
	Handle ctrl-C, ctrl-D and ctrl-\ which should behave like in bash.
	In interactive mode (shell):
		ctrl-C (SIGINT) displays a new prompt on a new line. exit status 1
		ctrl-\ (SIGQUIT) does nothing.
		ctrl-D exits the shell. This is not a signal. End Of File (EOF) on stdin

	In child process (within fork):
		SIGINT and SIGQUIT is set to default (SIG_DFL).
		Done to prevent the child from terminating when ctrl-C is pressed.

	In heredoc:
	child process:
		SIGINT is set to default (SIG_DFL).
		SIGQUIT is ignored (SIG_IGN).
	parent process:
		SIGINT is ignored (SIG_IGN) during waitpid().
		After waitpid() signal is handled,
			the parent process displays a new prompt on a new line.

	Summary of signal handling:
			Context	SIGINT          SIGQUIT
	Parent  shell	custom handler	ignored
	Normal  child	default         default
	Heredoc child	default         ignored
*/

/*
DESCRIPTION:
	Initializes the signals and sets the terminal to not echo control characters.
	When ECHOCTL flag is set, it prevents tty from printing control characters
	like ^C before handler runs.
*/

void	init_signals(struct termios *original_term)
{
	struct termios	term;

	if (setup_handler(SIGINT, shell_handler) == -1 || setup_handler(SIGQUIT,
			SIG_IGN) == -1)
		exit(1);
	if (!isatty(STDIN_FILENO))
		return ;
	if (tcgetattr(STDIN_FILENO, original_term) == -1)
		exit(1);
	term = *original_term;
	term.c_lflag &= ~ECHOCTL;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &term) == -1)
	{
		tcsetattr(STDIN_FILENO, TCSANOW, original_term);
		exit(1);
	}
}

/*
DESCRIPTION:
	Wrapper function that calls sigaction to set up a signal handler.
	On sigaction initialization error, Minishell should exit itself as a shell
	without proper signal handling is unsafe.

	Sigaction blocks SIGINT while processing SIGQUIT and vice-versa.
	To prevent this, SA_RESTART is used to restart the system call after the
	signal handler returns.

	Signature used for the sigaction handler:
		union __sigaction_u = void (*__sa_handler)(int);
*/

// should exit(EXIT_FAILURE); when setup_handler returns 1.
int	setup_handler(int sig, void (*handler)(int))
{
	struct sigaction	sa;

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
		return (perror(MINI), -1);
	return (0);
}

/*
DESCRIPTION:
	Parent signal handler sets the global signal variable and displays a new
	prompt on a new line and replaces the line with an empty string.

	Other option: faking input uding TIOCSTI (check man 2 ioctl_tty)
		ioctl(0, TIOCSTI, "\n");
	But this may also be signal unsafe and one more issue with this is it prints
	2 newlines to terminal, instead of 1 (not sure why)
*/

void	shell_handler(int sig)
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
DESCRIPTION:
	Handles the parent signal and sets the status to 1.
*/
void	handle_shell_signal(int *status)
{
	*status = 1;
	g_signal = 0;
}
