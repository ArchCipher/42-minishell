/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: kmurugan <kmurugan@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/28 19:45:20 by kmurugan          #+#    #+#             */
/*   Updated: 2026/01/01 14:10:48 by kmurugan         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal = 0;

/*
NOTES:
	Signal handling behavior:
	- Parent shell: SIGINT (custom handler), SIGQUIT (ignored)
	- Normal child: SIGINT/SIGQUIT (default)
	- Heredoc child: SIGINT (default), SIGQUIT (ignored)
	- Heredoc parent: SIGINT (ignored during waitpid)
*/

/*
DESCRIPTION:
	Sets the signal handlers for SIGINT and SIGQUIT.
	'sigint' is the handler for SIGINT and 'sigquit' is the handler for SIGQUIT.
	Returns 0 on success, 1 on failure.
*/

int	set_signal_handlers(void (*sigint)(int), void (*sigquit)(int))
{
	return ((set_signal_handler(SIGINT, sigint) == -1
			|| set_signal_handler(SIGQUIT, sigquit) == -1));
}

/*
DESCRIPTION:
	Wrapper function that calls sigaction to set up a signal handler.
	On sigaction initialization error, minishell should exit with failure as a
	shell without proper signal handling is unsafe.

	Only for custom handler (shell_handler):
		Sigaction blocks SIGQUIT while processing SIGINT. sa_flags is set to 0,
		so the system call is not restarted for readline().

	Signature used for the sigaction handler:
		union __sigaction_u = void (*__sa_handler)(int);
*/

int	set_signal_handler(int sig, void (*handler)(int))
{
	struct sigaction	sa;

	sa.sa_handler = handler;
	sigemptyset(&sa.sa_mask);
	if (handler != SIG_DFL && handler != SIG_IGN && sig == SIGINT)
		sigaddset(&sa.sa_mask, SIGQUIT);
	sa.sa_flags = 0;
	return (sigaction(sig, &sa, NULL));
}

/*
DESCRIPTION:
	Custom signal handler for parent shell. Sets the global signal variable and
	displays a new prompt on a new line and replaces the input line with an
	empty string.

	rl_replace_line, rl_on_new_line, rl_redisplay are not async-signal-safe,
	however, they are safe to use in signal handler if they interrupt readline
	itself as readline is designed for interactive shells that handle signals.

	Other option I tried: faking input using TIOCSTI (check man 2 ioctl_tty)
		ioctl(0, TIOCSTI, "\n");
	But ioctl is also not async-signal-safe and one more issue with this is
	that it prints 2 newlines to terminal, instead of 1.

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
	Also resets the global signal variable to 0.
*/

void	handle_shell_signal(int *status)
{
	*status = 1;
	g_signal = 0;
}
