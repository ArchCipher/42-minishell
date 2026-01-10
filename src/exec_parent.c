#include "minishell.h"

static int	setup_fds(int *o_stdin, int *o_stdout, int *o_stderr);
static void	restore_fds(int o_stdin, int o_stdout, int o_stderr,
				t_shell *shell);

/*
DESCRIPTION:
	Executes builtin in parent and restores standard input and output
	fds after execution.
NOTE:
	On exit from shell, "exit\n" should be written to stdout, before exec_exit
	is called. exec_exit() error messages should be printed after "exit\n".
*/

int	exec_in_parent(t_list *cmds, t_shell *shell)
{
	t_cmd	*cmd;
	int		o_stdin;
	int		o_stdout;
	int		o_stderr;
	int		saved_errno;

	if (setup_fds(&o_stdin, &o_stdout, &o_stderr))
		return (shell->status = 1, 1);
	cmd = get_cmd(cmds);
	if (setup_redirs(cmd->redirs, shell))
		return (restore_fds(o_stdin, o_stdout, o_stderr, shell), 1);
	if (isatty(o_stdout) && cmd->exec.builtin == BUILTIN_EXIT)
		write(o_stdout, "exit\n", 5);
	shell->status = exec_builtin(cmd, shell);
	saved_errno = errno;
	restore_fds(o_stdin, o_stdout, o_stderr, shell);
	if (cmd->exec.builtin == BUILTIN_EXIT && (!cmd->args[1] || (cmd->args[1]
				&& !cmd->args[2]) || (cmd->args[2] && saved_errno == EINVAL)))
		exit_shell(cmds, shell);
	return (shell->status);
}

static int	setup_fds(int *o_stdin, int *o_stdout, int *o_stderr)
{
	*o_stdout = dup(STDOUT_FILENO);
	if (*o_stdout == -1)
		return (perror(MINI), 1);
	*o_stdin = dup(STDIN_FILENO);
	if (*o_stdin == -1)
		return (close(*o_stdout), perror(MINI), 1);
	*o_stderr = dup(STDERR_FILENO);
	if (*o_stderr == -1)
		return (close(*o_stdout), close(*o_stdin), perror(MINI), 1);
	return (0);
}

/*
DESCRIPTION:
	Restores standard input and output fds after execution.
	Returns the exit status of the builtin execution or 1 on error.
*/

static void	restore_fds(int o_stdin, int o_stdout, int o_stderr, t_shell *shell)
{
	if ((dup2(o_stdin, STDIN_FILENO) == -1)
		|| (dup2(o_stdout, STDOUT_FILENO) == -1)
		|| (dup2(o_stderr, STDERR_FILENO) == -1))
	{
		perror(MINI);
		shell->status = 1;
	}
	close(o_stdin);
	close(o_stdout);
	close(o_stderr);
}
